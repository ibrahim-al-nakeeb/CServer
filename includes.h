//
//  inclides.h
//  Hw11
//
//  Created by abrahem elnkeeb on 21/01/2023.
//

#ifndef inclides_h
#define inclides_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define buffSize 256

#define usersFilePath "password.txt"

ssize_t readLine(char **lineptr, size_t *n, FILE *stream);

char* setRes(char* page, char* path);
char* getPage(const char* page);
char* getIndex();
char* signUP (char* user);
char* getHome(char* username);
char* loadHome(char* user);


int Login(const char* userName, const char* password);
int Register(const char* userName, const char* password);


#endif /* inclides_h */
