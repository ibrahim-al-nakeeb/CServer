//
//  user.c
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#include "user.h"

#define USERS_FILE "assets/db/users.txt"
#define TEMP_USERS_FILE "assets/db/users_tmp.txt"

// Splits a line into 3 parts: username, password, description
int parseUserLine(char *line, char **username, char **password, char **desc) {
	assert(line != NULL && username != NULL && password != NULL && desc != NULL);

	*username = strtok(line, ":");
	*password = strtok(NULL, ":");
	*desc = strtok(NULL, "\n");  // Up to end of line
	return (*username && *password && *desc);
}

// Allocates memory, caller must free result
char *getProfileDescription(const char *username) {
	assert(username != NULL);

	FILE *file = fopen(USERS_FILE, "r");
	if (!file) return NULL;

	static char line[MAX_LINE_LEN];
	while (fgets(line, sizeof(line), file)) {
		char *u, *p, *d;
		char temp[MAX_LINE_LEN];
		strcpy(temp, line);
		if (parseUserLine(temp, &u, &p, &d) && strcmp(u, username) == 0) {
			fclose(file);
			return strdup(d);
		}
	}

	fclose(file);
	return NULL;
}

int setProfileDescription(const char *username, const char *new_desc) {
	assert(username != NULL && new_desc != NULL);
	
	FILE *file = fopen(USERS_FILE, "r");
	if (!file) return USER_FILE_ERROR;

	FILE *temp = fopen(TEMP_USERS_FILE, "w");
	if (!temp) { 
		fclose(file);
		return USER_FILE_ERROR;
	}

	char line[MAX_LINE_LEN];
	int updated = 0;

	while (fgets(line, sizeof(line), file)) {
		char *u, *p, *d;
		char temp_line[MAX_LINE_LEN];
		strcpy(temp_line, line);

		if (parseUserLine(temp_line, &u, &p, &d) && strcmp(u, username) == 0) {
			fprintf(temp, "%s:%s:%s\n", u, p, new_desc);
			updated = 1;
		} else {
			fputs(line, temp);  // Copy original
		}
	}

	fclose(file);
	fclose(temp);

	remove(USERS_FILE);
	rename(TEMP_USERS_FILE, USERS_FILE);

	return updated ? UPDATE_SUCCESS : UPDATE_FAILED;
}

int checkPassword(const char *username, const char *password) {
	assert(username != NULL && password != NULL);

	FILE *file = fopen(USERS_FILE, "r");
	if (!file) return USER_FILE_ERROR;

	char line[MAX_LINE_LEN];
	while (fgets(line, sizeof(line), file)) {
		char *u, *p, *d;
		char temp[MAX_LINE_LEN];
		strcpy(temp, line);
		if (parseUserLine(temp, &u, &p, &d) && strcmp(u, username) == 0) {
			fclose(file);
			return strcmp(p, password) == 0;
		}
	}

	fclose(file);
	return PASSWORD_MISMATCH;
}

int checkUser(const char *username) {
	assert(username != NULL);

	FILE *file = fopen(USERS_FILE, "r");
	if (!file) return USER_FILE_ERROR;

	char line[MAX_LINE_LEN];
	while (fgets(line, sizeof(line), file)) {
		char *u, *p, *d;
		char temp[MAX_LINE_LEN];
		strcpy(temp, line);
		if (parseUserLine(temp, &u, &p, &d) && strcmp(u, username) == 0) {
			fclose(file);
			return USER_EXISTS;
		}
	}

	fclose(file);
	return USER_NOT_FOUND;
}

int addUser(const char *username, const char *password) {
	assert(username != NULL && password != NULL);

	// Runtime validation
	if (strlen(username) == 0 || strlen(password) == 0) {
		return ADD_USER_INVALID_INPUT;
	}

	FILE* file = fopen(USERS_FILE, "a");
	if (!file) {
		fprintf(stderr, "addUser error: could not open file for appending\n");
		return USER_FILE_ERROR;
	}

	fprintf(file, "%s:%s:no description\n", username, password);
	fclose(file);

	return ADD_USER_SUCCESS;
}