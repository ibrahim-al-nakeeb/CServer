//
//  user.h
//  CServer
//
//  Created by ibrahim alankeeb on 21/01/2023.
//

#ifndef user_h
#define user_h

#include <stdio.h>

#define UPDATE_SUCCESS 1
#define UPDATE_FAILED 0

#define USER_EXISTS 1
#define USER_NOT_FOUND 0

#define PASSWORD_MATCH 1
#define PASSWORD_MISMATCH 0


#define ADD_USER_SUCCESS 1
#define ADD_USER_INVALID_INPUT 0


#define MAX_LINE_LEN 512
#define USER_FILE_ERROR -1


int addUser(const char *username, const char *password);

int checkUser(const char *username);
int checkPassword(const char *username, const char *password);

char *getProfileDescription(const char *username);
int setProfileDescription(const char *username, const char *new_desc);


#endif /* user_h */