//
//  handlers.c
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#include "handlers.h"

#define ALERT(type, heading, message) \
    "<div class=\"alert alert-" type " alert-dismissible fade show\" role=\"alert\">" \
        "<strong>" heading "</strong> " message \
        "<button type=\"button\" class=\"btn-close\" data-bs-dismiss=\"alert\" aria-label=\"Close\"></button>" \
    "</div>"

void urlDecode(char *dst, const char *src) {
	assert(dst != NULL && src != NULL);

	while (*src) {
		if (*src == '%') {
			if (isxdigit(*(src + 1)) && isxdigit(*(src + 2))) {
				char hex[3] = { *(src + 1), *(src + 2), '\0' };
				*dst++ = (char)strtol(hex, NULL, 16);
				src += 3;
			} else {
				*dst++ = *src++;
			}
		} else if (*src == '+') {
			*dst++ = ' ';
			src++;
		} else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';
}

// Allocates memory, caller must free result
char *signUp(const char *payload) {
	assert(payload != NULL);
	char username[NAME_SIZE], password[NAME_SIZE];
	const char *placeholder[] = {"{{message}}"};
	const char *text[1];

	// Parse payload (format assumed: "username=...&password=...&profile=...")
	sscanf(payload, "username=%127[^&]&password=%127[^&]", username, password);

	int userStatus = checkUser(username);
	if (userStatus == USER_EXISTS) {
		text[0] = "someone else beat you to that name. Give it another shot!";
		char *template = renderTemplate(_400_PAGE, placeholder, text, 1);
		char *response = renderHtmlResponse(template);
		free(template);
		return response;
	} else if (userStatus == USER_FILE_ERROR) 
		return renderHtmlResponse("<h1>There was a server error. Please try again later.</h1>");

	// Try to add user
	int addStatus = addUser(username, password);
	if (addStatus == ADD_USER_SUCCESS) 
		return renderHtmlResponse("<h1>Sign-up successful! You can now sign in.</h1>");
	
	return renderHtmlResponse("<h1>Could not create account. Try again.</h1>");
}

void signIn(const char *payload) {
	if (!payload) {
		renderErrorPage("Invalid request payload.");
		return;
	}

	char username[NAME_SIZE], password[NAME_SIZE];
	sscanf(payload, "username=%127[^&]&password=%127[^\n]", username, password);

	int passwordStatus = checkPassword(username, password);
	if (passwordStatus == PASSWORD_MATCH) {
		char token[TOKEN_STRING_LENGTH + 1];
		generateToken(token, sizeof(token));
		storeSession(token, username);
		REDIRECT_WITH_SESSION("/home", token);
	} else if (passwordStatus == USER_FILE_ERROR) {
		renderErrorPage("Something went wrong on our end. Please try again later.");
	}

	const char *placeholders[] = { "alert" };
	const char *values[] = { ALERT("danger", "Unauthorized", "Invalid credentials.") };
	char *html = renderTemplate(LOGIN_PAGE, placeholders, values, 1);

	if(!html) {
		renderErrorPage("Unable to display login page.");
		return;
	}

	char *response = renderHtmlResponse(html, STATUS_401_UNAUTHORIZED);
	free(html);

	if (response) {
		printf("%s", response);
		free(response);
	} else {
		renderErrorPage("Something went wrong on our end. Please try again later.");
	}

}

// Ensures that the assets/db directory exists.
void setUp(void) {
	struct stat st = {0};
	if (stat("assets/db", &st) == -1) {
		mkdir("assets", 0755);
		mkdir("assets/db", 0755);
	}
}

void serveHomePage(const char *payload) {
	char *token = extractSessionToken();
	if (!token) {
		REDIRECT_AND_CLEAR_SESSION("/login");
		return;
	}

	char username[NAME_SIZE];
	if (getUsernameFromToken(token, username) != TOKEN_FOUND) {
		free(token);
		REDIRECT_AND_CLEAR_SESSION("/login");
		return;
	}

	free(token);

	if (payload) {
		const char *prefix = "profile-description=";
		if (strncmp(payload, prefix, strlen(prefix)) == 0) {
			const char *desc = payload + strlen(prefix);
			char decodedDesc[MAX_LINE_LEN];
			urlDecode(decodedDesc, desc);
			int result = setProfileDescription(username, decodedDesc);
			if (result != UPDATE_SUCCESS) {
				renderErrorPage("Unable to update profile description.");
				return;
			}
		}
	}

	const char *desc = getProfileDescription(username);
	if (!desc) {
		renderErrorPage("Unable to retrieve profile description.");
		return;
	}

	const char *placeholders[] = { "{{username}}", "{{profile}}" };
	const char *values[] = { username, desc };
	char *html = renderTemplate(HOME_PAGE, placeholders, values, 2);
	free(desc);

	if (!html) {
		renderErrorPage("Unable to display home page.");
		return;
	}

	char *response = renderHtmlResponse(html, STATUS_200_OK);
	free(html);

	if (response) {
		printf("%s", response);
		free(response);
	} else {
		renderErrorPage("Unable to display home page.");
	}
}

