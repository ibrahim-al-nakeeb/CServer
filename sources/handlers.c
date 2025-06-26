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

void signUp(const char *payload) {
	if (!payload) {
		renderErrorPage("Invalid request payload.");
		return;
	}

	char username[NAME_SIZE], password[NAME_SIZE];
	const char *placeholders[] = { "{{message}}" };
	const char *values[1];
	const char *status = STATUS_200_OK;

	// Parse payload (format assumed: "username=...&password=...&profile=...")
	sscanf(payload, "username=%127[^&]&password=%127[^&]", username, password);

	int userStatus = checkUser(username);
	if (userStatus == USER_EXISTS) {
		values[0] = ALERT("danger", "Oops!", "That name has already been registered. Select a different one.");
		status = STATUS_400_BAD_REQUEST;
	} else if (userStatus == USER_FILE_ERROR) {
		renderErrorPage("Something went wrong on our end. Please try again later.");
		return;
	}

	// Try to add user
	int addStatus = addUser(username, password);
	if (addStatus == ADD_USER_SUCCESS) {
		values[0] = ALERT("success", "Sign-up successful!", "You're all set! Go ahead and sign in.");
	} else {
		renderErrorPage("Oops! We couldn’t create your account. Give it another try.");
		return;
	}

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
		return;
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

void serveLoginPage() {
	char* token = extractSessionToken();
	if (token) {
		int status = checkToken(token);
		free(token);

		if (status == TOKEN_VALID) {
			REDIRECT("/home");
			return;
		}
	}

	const char *placeholders[] = { "alert" };
	const char *values[] = { "" };

	char *html = renderTemplate(LOGIN_PAGE, placeholders, values, 1);
	if(!html) {
		renderErrorPage("Unable to display login page.");
		return;
	}

	char *response = renderHtmlResponse(html, STATUS_200_OK);
	free(html);

	if (response) {
		printf("%s", response);
		free(response);
	} else {
		renderErrorPage("Something went wrong on our end. Please try again later.");
	}
}

void send404Page() {
	char *response = renderFileResponse(_404_PAGE, NULL);
	if (response) {
		printf("%s", response);
		free(response);
	}
}

void sendFileResponse(const char *filePath) {
	int response_size = 0;
	char *response = renderFileResponse(filePath, &response_size);
	if (response) {
		fwrite(response, 1, response_size, stdout);
		free(response);
	}
}
