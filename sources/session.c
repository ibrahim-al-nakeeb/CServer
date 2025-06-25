//
//  session.c
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#include "session.h"

#define SESSIONS_FILE "assets/db/sessions.txt"

int generateToken(char *token, size_t length) {
	assert(token != NULL);
	assert(length >= TOKEN_STRING_LENGTH);

	unsigned char buffer[TOKEN_BYTE_LENGTH];

	if (RAND_bytes(buffer, TOKEN_BYTE_LENGTH) != 1) {
		fprintf(stderr, "Failed to generate secure random bytes\n");
		return TOKEN_GENERATION_FAILURE;
	}

	for (size_t i = 0; i < TOKEN_BYTE_LENGTH; ++i) {
		sprintf(&token[i * 2], "%02x", buffer[i]);
	}

	token[TOKEN_STRING_LENGTH - 1] = '\0';
	return TOKEN_GENERATION_SUCCESS;
}

int getUsernameFromToken(const char *token, char *outUsername) {
	assert(token != NULL && outUsername != NULL);

	FILE* file = fopen(SESSIONS_FILE, "r");
	if (!file) return TOKEN_FILE_ERROR;

	char line[SESSION_LINE_LEN];
	while (fgets(line, sizeof(line), file)) {
		char savedToken[TOKEN_STRING_LENGTH], savedUsername[NAME_SIZE];
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
		char savedToken[TOKEN_STRING_LENGTH];
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
