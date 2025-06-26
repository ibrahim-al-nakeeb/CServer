//
//  handlers.h
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#ifndef handlers_h
#define handlers_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "user.h"
#include "session.h"
#include "response.h"


void setUp(void);

void signUp(const char *payload);
void signIn(const char *payload);

void send404Page();
void serveLoginPage();
void serveHomePage(const char *payload);
void sendFileResponse(const char *filePath);


#endif /* handlers_h */
