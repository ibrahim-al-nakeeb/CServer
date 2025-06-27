//
//  session.c
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#include "session.h"

#define SESSIONS_FILE "assets/db/sessions.txt"

/*
 * Generates a secure random token and stores it as a hexadecimal string.
 *
 * Parameters:
 *   token - Output buffer where the generated token string will be stored.
 *           Must be pre-allocated with at least TOKEN_BYTE_LENGTH bytes.
 *
 * Returns:
 *   TOKEN_GENERATION_SUCCESS on successful token generation,
 *   TOKEN_GENERATION_FAILURE if secure random bytes could not be generated.
 */
int generateToken(char *token) {
	assert(token != NULL);

	unsigned char buffer[TOKEN_SIZE];

	if (RAND_bytes(buffer, TOKEN_SIZE) != 1) {
		fprintf(stderr, "Failed to generate secure random bytes\n");
		return TOKEN_GENERATION_FAILURE;
	}

	for (size_t i = 0; i < TOKEN_SIZE; ++i) {
		sprintf(&token[i * 2], "%02x", buffer[i]);
	}

	token[TOKEN_BYTE_LENGTH - 1] = '\0';
	return TOKEN_GENERATION_SUCCESS;
}

/*
 * Retrieves the username associated with a given session token from the SESSIONS_FILE.
 *
 * Parameters:
 *   token        - The session token to look up (must not be NULL).
 *   outUsername  - Output buffer to store the corresponding username (must not be NULL).
 *
 * Returns:
 *   TOKEN_FOUND if a matching token is found and the username is copied to outUsername,
 *   TOKEN_NOT_FOUND if the token does not exist in the session file,
 *   TOKEN_FILE_ERROR if the session file could not be opened.
 */
int getUsernameFromToken(const char *token, char *outUsername) {
	assert(token != NULL && outUsername != NULL);

	FILE* file = fopen(SESSIONS_FILE, "r");
	if (!file) return TOKEN_FILE_ERROR;

	char line[SESSION_LINE_LEN];
	while (fgets(line, sizeof(line), file)) {
		char savedToken[TOKEN_BYTE_LENGTH], savedUsername[NAME_SIZE];
		if (sscanf(line, "%64[^:]:%127s", savedToken, savedUsername) == 2) {
			if (strcmp(token, savedToken) == 0) {
				strcpy(outUsername, savedUsername);
				fclose(file);
				return TOKEN_FOUND;
			}
		}
	}

	fclose(file);
	return TOKEN_NOT_FOUND;
}

/*
 * Stores a new session by appending the token and associated username to the SESSIONS_FILE.
 *
 * Parameters:
 *   token    - The session token to store (must not be NULL).
 *   username - The username associated with the token (must not be NULL).
 *
 * Returns:
 *   SESSION_WRITE_SUCCESS if the session was successfully stored,
 *   SESSION_WRITE_FAILED if the session file could not be opened for appending.
 */
int storeSession(const char *token, const char *username) {
	assert(token != NULL && username != NULL);

	FILE* file = fopen(SESSIONS_FILE, "a");
	if (!file) return SESSION_WRITE_FAILED;

	fprintf(file, "%s:%s\n", token, username);
	fclose(file);

	return SESSION_WRITE_SUCCESS;
}

/*
 * Checks if a given session token exists in the SESSIONS_FILE.
 *
 * Parameters:
 *   token - The session token to validate (must not be NULL).
 *
 * Returns:
 *   TOKEN_VALID if the token is found,
 *   TOKEN_INVALID if the token is not found or is NULL,
 *   TOKEN_FILE_ERROR if the session file could not be opened.
 */
int checkToken(const char *token) {
	assert(token != NULL);
	if (!token) return TOKEN_INVALID;

	FILE* file = fopen(SESSIONS_FILE, "r");
	if (!file) return TOKEN_FILE_ERROR;

	char line[SESSION_LINE_LEN];
	while (fgets(line, sizeof(line), file)) {
		char savedToken[TOKEN_BYTE_LENGTH];
		char username[NAME_SIZE];

		if (sscanf(line, "%64[^:]:%127s", savedToken, username) == 2) {
			if (strcmp(savedToken, token) == 0) {
				fclose(file);
				return TOKEN_VALID;
			}
		}
	}

	fclose(file);
	return TOKEN_INVALID;
}

/*
 * Extracts the session token from the "Cookie" HTTP header.
 *
 * Returns:
 *   A newly allocated string containing the session token if found and valid,
 *   or NULL if the "Cookie" header is missing, malformed, or memory allocation fails.
 */
char *extractSessionToken() {
	const char *cookieHeader = request_header("Cookie");
	if (!cookieHeader) return NULL;

	const char *sessionPrefix = "session=";
	const char *sessionStart = strstr(cookieHeader, sessionPrefix);
	if (!sessionStart) return NULL;

	sessionStart += strlen(sessionPrefix);
	const char *sessionEnd = strchr(sessionStart, ';'); // end at ';' or end of string

	size_t len = sessionEnd ? (size_t)(sessionEnd - sessionStart) : strlen(sessionStart);
	if (len == 0 || len > TOKEN_BYTE_LENGTH) return NULL;

	char *token = malloc(len + 1);
	if (!token) return NULL;

	strncpy(token, sessionStart, len);
	token[len] = '\0';
	return token;
}
