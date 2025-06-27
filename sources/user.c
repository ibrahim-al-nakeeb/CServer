//
//  user.c
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#include "user.h"

#define USERS_FILE "assets/db/users.txt"
#define TEMP_USERS_FILE "assets/db/users_tmp.txt"

/*
 * Splits a line of format "username:password:description" into its parts.
 *
 * Parameters:
 *   line     - Writable input string (will be modified).
 *   username - Output pointer to the username.
 *   password - Output pointer to the password.
 *   desc     - Output pointer to the description.
 *
 * Returns:
 *   1 on success, 0 if any part is missing.
 */
int parseUserLine(char *line, char **username, char **password, char **desc) {
	assert(line != NULL && username != NULL && password != NULL && desc != NULL);

	*username = strtok(line, ":");
	*password = strtok(NULL, ":");
	*desc = strtok(NULL, "\n");  // Up to end of line
	return (*username && *password && *desc);
}

/*
 * Searches for a user in the USERS_FILE and returns a copy of their profile description.
 *
 * Parameters:
 *   username - The username to search for (must not be NULL).
 *
 * Returns:
 *   A newly allocated string containing the profile description if the user is found,
 *   or NULL if the user is not found or if an error occurs while opening the file.
 *
 * Side Effects:
 *   Allocates memory for the returned description string, which must be freed by the caller.
 */
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

/*
 * Updates the profile description of the specified user in the USERS_FILE.
 *
 * Parameters:
 *   username  - The username whose description is to be updated (must not be NULL).
 *   new_desc  - The new description to set (must not be NULL).
 *
 * Returns:
 *   UPDATE_SUCCESS (1) if the description was successfully updated,
 *   UPDATE_FAILED (0) if the user was not found,
 *   USER_FILE_ERROR (-1) if there was an error opening the user file or temporary file.
 */
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

/*
 * Verifies whether the provided password matches the stored password for a given username.
 *
 * Parameters:
 *   username - The username to authenticate (must not be NULL).
 *   password - The password to verify (must not be NULL).
 *
 * Returns:
 *   1 if the username exists and the password matches,
 *   0 if the username exists but the password does not match,
 *   USER_FILE_ERROR if the user file could not be opened.
 */
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

/*
 * Checks if a user with the specified username exists in the USERS_FILE.
 *
 * Parameters:
 *   username - The username to search for (must not be NULL).
 *
 * Returns:
 *   USER_EXISTS if the user is found,
 *   USER_NOT_FOUND if the user does not exist,
 *   USER_FILE_ERROR if the user file could not be opened.
 */
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

/*
 * Adds a new user with the given username and password to the USERS_FILE.
 * A default description ("no description") is assigned.
 *
 * Parameters:
 *   username - The username to add (must not be NULL or empty).
 *   password - The password for the new user (must not be NULL or empty).
 *
 * Returns:
 *   ADD_USER_SUCCESS if the user was successfully added,
 *   ADD_USER_INVALID_INPUT if username or password is empty,
 *   ADD_USER_FAILED if the user already exists,
 *   USER_FILE_ERROR if the file could not be opened for writing.
 */
int addUser(const char *username, const char *password) {
	assert(username != NULL && password != NULL);

	// Runtime validation
	if (strlen(username) == 0 || strlen(password) == 0) {
		return ADD_USER_INVALID_INPUT;
	}

	if(checkUser(username) == USER_EXISTS) return ADD_USER_FAILED;

	FILE* file = fopen(USERS_FILE, "a");
	if (!file) {
		fprintf(stderr, "addUser error: could not open file for appending\n");
		return USER_FILE_ERROR;
	}

	fprintf(file, "%s:%s:no description\n", username, password);
	fclose(file);

	return ADD_USER_SUCCESS;
}