//
//  session.h
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#ifndef session_h
#define session_h

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <openssl/rand.h>
#include <sys/stat.h>

#include "httpd.h"

#define TOKEN_SIZE 32
#define NAME_SIZE 128
#define SESSION_LINE_LEN 256

#define TOKEN_BYTE_LENGTH ((TOKEN_SIZE) * 2 + 1)

#define TOKEN_FOUND 1
#define TOKEN_NOT_FOUND 0
#define TOKEN_FILE_ERROR -1

#define SESSION_WRITE_SUCCESS 1
#define SESSION_WRITE_FAILED 0

#define TOKEN_GENERATION_SUCCESS 1
#define TOKEN_GENERATION_FAILURE 0

#define TOKEN_VALID 1
#define TOKEN_INVALID 0

char *extractSessionToken();
int checkToken(const char *token);
int generateToken(char *token);
int storeSession(const char *token, const char *username);
int getUsernameFromToken(const char *token, char *outUsername);

#endif /* session_h */
