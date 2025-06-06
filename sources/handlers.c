//
//  handlers.c
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#include "handlers.h"

void replce(char* str, char c, char rep) {
	assert(str != NULL);

	long sz = strlen(str);
	for (int i = 0; i < sz ; i++)
		if (str[i] == c) str[i] = rep;
}

// Allocates memory, caller must free result
char* signUp(const char* payload) {
	assert(payload != NULL);
	char username[NAME_SIZE], password[NAME_SIZE];
	const char *placeholder[] = {"{{message}}"};
	const char *text[1];

	// Parse payload (format assumed: "username=...&password=...&profile=...")
	sscanf(payload, "username=%127[^&]&password=%127[^&]", username, password);

	int userStatus = checkUser(username);
	if (userStatus == USER_EXISTS) {
		text[0] = "someone else beat you to that name. Give it another shot!";
		char* template = renderTemplate(_400_PAGE, placeholder, text, 1);
		char* response = renderHtmlResponse(template);
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


// Allocates memory, caller must free result
char* signIn(const char* payload) {
	char username[NAME_SIZE], password[NAME_SIZE];
	sscanf(payload, "username=%127[^&]&password=%127[^\n]", username, password);

	int passwordStatus = checkPassword(username, password);
	if (passwordStatus == PASSWORD_MATCH) {
		char token[TOKEN_SIZE + 1];
		generateToken(token, sizeof(token));
		storeSession(token, username);

		char* result = malloc(MAX_LINE_LEN);
		snprintf(result, MAX_LINE_LEN,
			"HTTP/1.1 302 Found\r\n"
			"Location: /home\r\n"
			"Set-Cookie: session=%s; Max-Age=3600; HttpOnly\r\n"
			"Content-Length: 0\r\n"
			"\r\n",
			token
		);
		return result;
	} else if (passwordStatus == USER_FILE_ERROR)
		return renderHtmlResponse("<h1>There was a server error. Please try again later.</h1>");

	return strdup("HTTP/1.1 401 Unauthorized\r\n\r\nInvalid credentials.");
}

char* signIn(const char* payload) {
	char username[NAME_SIZE], password[NAME_SIZE];

	// Parse payload (format assumed: "username=...&password=...")
	sscanf(payload, "username=%127[^&]&password=%127[^\n]", username, password);

	if (checkPassword(username, password)) {
		char token[TOKEN_SIZE];
		generateSecureToken(token, sizeof(token));
		char* profile = getProfileDescription(username);
		char* response = (char*)malloc(1024);
		snprintf(response, 1024,
			"<h1>Welcome, %s!</h1><p>Your profile:</p><pre>%s</pre>",
			username, profile ? profile : "No description.");
		free(profile);
		return response;
	}
	
	const char *placeholder[] = {"{{message}}"};
	const char *text[1];

	*text = "you got the username or the password wrong.";
	char* template = renderTemplate(_400_PAGE, placeholder, text, 1);
	char* response = renderHtmlResponse(template);
	free(template);
	 return response;
}


// Ensures that the assets/db directory exists.
void setUp(void) {
	struct stat st = {0};
	if (stat("assets/db", &st) == -1) {
		mkdir("assets", 0755);
		mkdir("assets/db", 0755);
	}
}
