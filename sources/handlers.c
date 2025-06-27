//
//  handlers.c
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#include "handlers.h"

/*
 * Macro to generate a Bootstrap-styled alert HTML string with a close button.
 *
 * Parameters:
 *   type    - The alert type (e.g., "success", "danger", "warning").
 *   heading - The bold heading text displayed at the start of the alert.
 *   message - The main message content following the heading.
 *
 * Returns:
 *   A string literal representing the complete HTML for the alert box.
 */
#define ALERT(type, heading, message) \
    "<div class=\"alert alert-" type " alert-dismissible fade show\" role=\"alert\">" \
        "<strong>" heading "</strong> " message \
        "<button type=\"button\" class=\"btn-close\" data-bs-dismiss=\"alert\" aria-label=\"Close\"></button>" \
    "</div>"

/*
 * Decodes a URL-encoded string, converting percent-encoded characters and plus signs.
 *
 * Parameters:
 *   dst - Destination buffer to store the decoded result (must not be NULL).
 *         Must be large enough to hold the decoded string.
 *   src - Source URL-encoded string (must not be NULL).
 *
 * Behavior:
 *   - Replaces "%XX" sequences with their corresponding character (hexadecimal decoding).
 *   - Replaces '+' characters with spaces.
 *   - Copies all other characters as-is.
 */
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

/*
 * Handles user sign-up by parsing the request payload, creating a new user, and responding with feedback.
 *
 * Parameters:
 *   payload - A URL-encoded string containing "username" and "password" parameters (must not be NULL).
 *
 * Behavior:
 *   - Parses the payload to extract credentials.
 *   - Attempts to register the user using addUser().
 *   - On success, returns a login page with a success alert.
 *   - On failure due to duplicate username, returns the login page with an error alert.
 *   - On internal error, displays an error page.
 *
 * Side Effects:
 *   Prints the HTTP response to stdout.
 */
void signUp(const char *payload) {
	if (!payload) {
		renderErrorPage("Invalid request payload.");
		return;
	}

	char username[NAME_SIZE], password[NAME_SIZE];
	const char *placeholders[] = { "{{alert}}" };
	const char *values[1];
	const char *status = STATUS_200_OK;

	// Parse payload (format assumed: "username=...&password=...&profile=...")
	sscanf(payload, "username=%127[^&]&password=%127[^&]", username, password);
	int addStatus = addUser(username, password);
	if (addStatus == ADD_USER_SUCCESS) {
		values[0] = ALERT("success", "Sign-up successful!", "You're all set! Go ahead and sign in.");
	} else if (addStatus == ADD_USER_FAILED) {
		values[0] = ALERT("danger", "Oops!", "That name has already been registered. Select a different one.");
		status = STATUS_400_BAD_REQUEST;
	} else {
		renderErrorPage("Something went wrong on our end. Please try again later.");
		return;
	}

	char *html = renderTemplate(LOGIN_PAGE, placeholders, values, 1);
	if(!html) {
		renderErrorPage("Unable to display login page.");
		return;
	}

	char *response = renderHtmlResponse(html, status);
	free(html);

	if (response) {
		printf("%s", response);
		free(response);
	} else {
		renderErrorPage("Something went wrong on our end. Please try again later.");
	}
}

/*
 * Handles user sign-in by validating credentials and initiating a session on success.
 *
 * Parameters:
 *   payload - A URL-encoded string containing "username" and "password" parameters (must not be NULL).
 *
 * Behavior:
 *   - Parses the payload to extract login credentials.
 *   - Validates credentials using checkPassword().
 *   - On success, generates a session token, stores it, and redirects to /home with a session cookie.
 *   - On invalid credentials, renders the login page with an error alert.
 *   - On file errors, renders an internal error page.
 *
 * Side Effects:
 *   Generates and store a session token.
 *   Prints the HTTP response to stdout.
 */
void signIn(const char *payload) {
	if (!payload) {
		renderErrorPage("Invalid request payload.");
		return;
	}

	char username[NAME_SIZE], password[NAME_SIZE];
	sscanf(payload, "username=%127[^&]&password=%127[^\n]", username, password);

	int passwordStatus = checkPassword(username, password);
	if (passwordStatus == PASSWORD_MATCH) {
		char token[TOKEN_BYTE_LENGTH];
		generateToken(token);
		storeSession(token, username);
		REDIRECT_WITH_SESSION("/home", token);
	} else if (passwordStatus == USER_FILE_ERROR) {
		renderErrorPage("Something went wrong on our end. Please try again later.");
		return;
	}

	const char *placeholders[] = { "{{alert}}" };
	const char *values[] = { ALERT("danger", "Unauthorized!", "Invalid credentials.") };
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

/*
 * Ensures that the "assets/db" directory exists, creating it if necessary.
 *
 * Behavior:
 *   - Checks if "assets/db" exists using stat().
 *   - If not present, creates the "assets" and "assets/db" directories with 0755 permissions.
 *
 * Side Effects:
 *   Creates directories on the filesystem.
 */
void setUp(void) {
	struct stat st = {0};
	if (stat("assets/db", &st) == -1) {
		mkdir("assets", 0755);
		mkdir("assets/db", 0755);
	}
}

/*
 * Serves the home page for a signed-in user, optionally updating the user's profile description.
 *
 * Parameters:
 *   payload - Optional URL-encoded form data. If present and begins with "profile-description=",
 *             the user's profile description will be updated accordingly.
 *
 * Behavior:
 *   - Extracts and validates the session token from the Cookie header.
 *   - Retrieves the associated username from the session token.
 *   - If a valid profile update payload is provided, decodes and saves the new description.
 *   - Loads and renders the home page template with the user's username and description.
 *   - Responds with a full HTML response or an error page if any step fails.
 *
 * Side Effects:
 *   Sends the HTTP response to stdout.
 *   Redirects to the login page and clears the session on invalid token.
 */
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

	char *desc = getProfileDescription(username);
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

/*
 * Serves the login page, redirecting authenticated users to the home page.
 *
 * Behavior:
 *   - Attempts to extract and validate a session token from the Cookie header.
 *   - If the token is valid, redirects the user to "/home".
 *   - Otherwise, renders and serves the login page without any alert messages.
 *   - If rendering fails, displays an internal error page.
 *
 * Side Effects:
 *   Sends the HTTP response to stdout.
 */
void serveLoginPage() {
	char *token = extractSessionToken();
	if (token) {
		int status = checkToken(token);
		free(token);

		if (status == TOKEN_VALID) {
			REDIRECT("/home");
			return;
		}
	}

	const char *placeholders[] = { "{{alert}}" };
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

/*
 * Sends a pre-rendered 404 Not Found HTML page as the HTTP response.
 *
 * Behavior:
 *   - Loads the contents of the _404_PAGE file using RENDER_FILE.
 *   - Prints the response to stdout if successfully loaded.
 *
 * Side Effects:
 *   Sends the HTTP response to stdout.
 */
void send404Page() {
	char *response = RENDER_FILE(_404_PAGE);
	if (response) {
		printf("%s", response);
		free(response);
	}
}

/*
 * Sends the contents of the specified file as an HTTP response.
 *
 * Parameters:
 *   filePath - Path to the file to be served (must not be NULL).
 *
 * Behavior:
 *   - Loads the file using RENDER_FILE_WITH_SIZE, which includes HTTP headers.
 *   - Writes the complete response to stdout using fwrite.
 *   - Frees the allocated memory after sending.
 *
 * Side Effects:
 *   Sends the HTTP response to stdout.
 */
void sendFileResponse(const char *filePath) {
	int response_size = 0;
	char *response = RENDER_FILE_WITH_SIZE(filePath, &response_size);
	if (response) {
		fwrite(response, 1, response_size, stdout);
		free(response);
	}
}

/*
 * Handles POST requests to the login endpoint by dispatching to sign-in or sign-up logic.
 *
 * Parameters:
 *   payload - A URL-encoded request body containing an "action" key and additional parameters (must not be NULL).
 *
 * Behavior:
 *   - Parses the "action" field from the payload.
 *   - If action is "signin", delegates to signIn() with the remaining payload.
 *   - If action is "signup", delegates to signUp() with the remaining payload.
 *   - Otherwise, renders an error page indicating an invalid request.
 *
 * Side Effects:
 *   Generates and send HTML responses to stdout.
 */
void handleLoginPost(const char *payload) {
	if (!payload) {
		renderErrorPage("Invalid request payload.");
		return;
	}

	char action[16] = {0};
	sscanf(payload, "action=%15[^&]", action);

	size_t prefixLen = strlen("action=") + strlen(action) + 1; // +1 for '&'

	if (strcmp(action, "signin") == 0) {
		signIn(payload + prefixLen);
	} else if (strcmp(action, "signup") == 0) {
		signUp(payload + prefixLen);
	} else {
		renderErrorPage("Invalid request action.");
	}
}
