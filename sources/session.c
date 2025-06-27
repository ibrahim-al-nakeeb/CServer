//
//  session.c
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#include "session.h"

#define SESSIONS_FILE "assets/db/sessions.txt"

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

int getUsernameFromToken(const char *token, char *outUsername) {
	assert(token != NULL && outUsername != NULL);

	FILE* file = fopen(SESSIONS_FILE, "r");
	if (!file) return TOKEN_FILE_ERROR;

	char line[SESSION_LINE_LEN];
	while (fgets(line, sizeof(line), file)) {
		char savedToken[TOKEN_SIZE], savedUsername[NAME_SIZE];
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

int storeSession(const char *token, const char *username) {
	assert(token != NULL && username != NULL);

	FILE* file = fopen(SESSIONS_FILE, "a");
	if (!file) return SESSION_WRITE_FAILED;

	fprintf(file, "%s:%s\n", token, username);
	fclose(file);

	return SESSION_WRITE_SUCCESS;
}

int checkToken(const char *token) {
	assert(token != NULL);
	if (!token) return TOKEN_INVALID;

	FILE* file = fopen(SESSIONS_FILE, "r");
	if (!file) return TOKEN_FILE_ERROR;

	char line[SESSION_LINE_LEN];
	while (fgets(line, sizeof(line), file)) {
		char savedToken[TOKEN_SIZE];
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

char *extractSessionToken() {
	const char* cookieHeader = request_header("Cookie");
	if (!cookieHeader) return NULL;

	const char* sessionPrefix = "session=";
	const char* sessionStart = strstr(cookieHeader, sessionPrefix);
	if (!sessionStart) return NULL;

	sessionStart += strlen(sessionPrefix);
	const char* sessionEnd = strchr(sessionStart, ';'); // end at ';' or end of string

	size_t len = sessionEnd ? (size_t)(sessionEnd - sessionStart) : strlen(sessionStart);
	if (len == 0 || len > TOKEN_SIZE) return NULL;

	char* token = malloc(len + 1);
	if (!token) return NULL;

	strncpy(token, sessionStart, len);
	token[len] = '\0';
	return token;
}
