//
//  session.h
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#ifndef session_h
#define session_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>


#define TOKEN_SIZE 64
#define NAME_SIZE 128
#define SESSION_LINE_LEN 256

#define TOKEN_FOUND 1
#define TOKEN_NOT_FOUND 0
#define TOKEN_FILE_ERROR -1

#define SESSION_WRITE_SUCCESS 1
#define SESSION_WRITE_FAILED 0

#define TOKEN_GENERATION_SUCCESS 1
#define TOKEN_GENERATION_FAILURE 0

#define TOKEN_BYTE_LENGTH 32								// 32 bytes = 64 hex characters
#define TOKEN_STRING_LENGTH (TOKEN_BYTE_LENGTH * 2 + 1)		// +1 for null terminator



int generateSecureToken(char *token, size_t length);
int storeSession(const char* token, const char* username);
int getUsernameFromToken(const char* token, char* outUsername);

#endif /* session_h */
