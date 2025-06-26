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

#include "user.h"
#include "session.h"
#include "response.h"


void setUp(void);

char *signUp(const char *payload);
char *signIn(const char *payload);

void serveHomePage(const char* payload);


#endif /* handlers_h */
