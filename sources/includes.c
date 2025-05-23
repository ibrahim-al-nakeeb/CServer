//
//  inclides.c
//  Hw11
//
//  Created by abrahem elnkeeb on 21/01/2023.
//

#include "includes.h"

char* nane = "";

void replce(char* str, char c, char rep) {
    long sz = strlen(str);
    for (int i = 0; i < sz ; i++)
        if (str[i] == c)
            str[i] = rep;
}

char* splitUser(char* str) {

    if (str == NULL) 
        return NULL;
    
    char dilmeter[] = "-";
    return strtok(str, dilmeter);
}

char* getData(char* username) {

    if(username == NULL)
        return nane;
    
    FILE * fPtr = fopen("data.txt", "r");
    char dilmeter[] = "-", *buffer = NULL;
    char *token;

    if (fPtr == NULL) 
        return nane;
    

    size_t sz;
    while (readLine(&buffer, &sz, fPtr) != -1) {
        token = strtok(buffer, dilmeter);
        if (token != NULL && strcmp(token, username) == 0) {
            token = strtok(NULL, dilmeter);
            char* temp = strdup(token);
            free(buffer);
            return temp;
        }
    }

    free(buffer);
    return nane;
}

char* setData(char* username, char* data) {

    if(username == NULL || data == NULL)
        return NULL;
    
    FILE * fPtr;
    FILE * fTemp;

    int flag = 0;

    char* buffer = NULL, *token, *temp;
    char  newline[buffSize], dilmeter[] = "-";
    

    /* Remove extra new line character from stdin */
    //fflush(stdin);

    fPtr  = fopen("data.txt", "r");
    fTemp = fopen("tmpData.txt", "a"); 

    if (fPtr == NULL || fTemp == NULL) 
        return getHome(username);
    

    size_t sz;
    while (readLine(&buffer, &sz, fPtr) != -1) {
        temp = strdup(buffer);
        token = strtok(buffer, dilmeter);
        if (strcmp(token, username) == 0) {
            fprintf(fTemp, "%s-%s\n", username, data);
            flag = 1;
        }    
        else
            fprintf(fTemp, "%s\n", temp);
        free(temp);
    }
    free(buffer);

    if (!flag)
        fprintf(fTemp, "%s-%s\n", username, data);
    
    fclose(fPtr);
    fclose(fTemp);

    /* Delete original source file */
    remove("data.txt");

    /* Rename temporary file as original file */
    rename("tmpData.txt", "data.txt");

    return getHome(username);

}

int Login(const char* userName, const char* password) {
    
    if(userName == NULL || password == NULL)
        return -1;
    
    char dilmeter[] = "-", *buffer = NULL;
    char *token;
    
    FILE* users = fopen(usersFilePath, "r");
    
    if(users == NULL) {
          perror("Error opening file");
          return -1;
    }
    
    size_t sz;
    while (readLine(&buffer, &sz, users) != -1) {
        token = strtok(buffer, dilmeter);
        if(strcmp(token, userName) == 0) {
            token = strtok(NULL, dilmeter);
            if(strcmp(token, password) == 0) {
                free(buffer);
                fclose(users);
                return 1;
            }
            else {
                free(buffer);
                fclose(users);
                return -2;
            }
               
        }
    }
    free(buffer);
    fclose(users);
    return 0;
}


int Register(const char* userName, const char* password) {
    
    int result = Login(userName,password);
    
    if(result == 1 || result == -2)
        return -1;
    
    FILE* users = fopen(usersFilePath, "a");
    
    if(users == NULL) {
          perror("Error opening file");
          return -2;
    }
    
    fclose(users);
    return 0;
}


ssize_t readLine(char **lineptr, size_t *n, FILE *stream) {
    
    int c;
    size_t i = 0;
    
    if (*lineptr == NULL) {
        *lineptr = malloc(sizeof(char) * buffSize);
        *n = buffSize;
    }
    
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        (*lineptr)[i++] = (char) c;
        if (i == *n) {
            *lineptr = realloc(*lineptr, sizeof(char) * *n * 2);
            *n *= 2;
        }
    }
    
    (*lineptr)[i] = '\0';
    if (c == EOF && i == 0)
        return -1;
    
    return i;
    
}


char* getPage(const char* page) {

    if(page == NULL)
        return NULL;
    
    FILE *file = fopen(page, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    // Get the size of the file
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);

     // Allocate memory to store the file contents
    char *html = (char*)malloc(file_size * sizeof(char));

    // Read the contents of the file
    fread(html, sizeof(char), file_size, file);

    // Close the file
    fclose(file);

    return html;

}

char* setRes(char* page, char* status) {

    if (page == NULL) {
        return NULL;
    }

    // Create the HTTP response headers
    char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\n\n%s";
    if (status != NULL && strcmp(status,"404") == 0)
    {
        response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: %d\n\n%s";
    }
    else if (status != NULL && strcmp(status, "400") == 0)
    {
        response = "HTTP/1.1 400 Bad Request\nContent-Type: text/html\nContent-Length: %d\n\n%s";
    }
    
    long pageSz = strlen(page);

    char* buffer = (char*)malloc((pageSz + buffSize) * sizeof(char));;
    sprintf(buffer, response, pageSz, page);
    free(page);
    return buffer;
}

char* replaceSubString(char* str, char* substr, char* replace) {

        if (str == NULL)
            return NULL;
        if (substr == NULL || replace == NULL)
            return str;
        
        int i = 0, j = 0, flag = 0, start = 0;
        char* result;
        
        
        long strSz = strlen(str), resSz;
        long diff = strlen(substr) - strlen(replace);

        if (diff < 0 ) {
            resSz = diff + strSz + 1;
        }
        else {
            resSz = strSz + 1;
        }
        result = (char*) malloc(sizeof(char)*resSz);

        if (result == NULL)
            return str;
        
        // check whether the substring to be replaced is present 
        while (str[i] != '\0')
        {
            if (str[i] == substr[j]) 
            {
                if (!flag)
                    start = i;
                j++;
                if (substr[j] == '\0')
                break;
                flag = 1;
            } 
            else 
            {
                flag = start = j = 0;
            }
            i++;
        }
        if (substr[j] == '\0' && flag)
        {
            for (i = 0; i < start; i++)
                result[i] = str[i];

            // replace substring with another string 
            for (j = 0; j < strlen(replace); j++) 
            {
                result[i] = replace[j];
                i++;
            }
            // copy remaining portion of the input string "str" 
            for (j = start + strlen(substr); j < strlen(str); j++)
            {
                result[i] = str[j];
                i++;
            }
            result[i] = '\0';
                
        }
        free(str);
        return result;
}


char* loadHome(char* user) {

    if(user == NULL)
        return NULL;
    
    char* token = strtok(user, "&"), *username, *second;
    char* nameParam = token, *secondParam = strtok(NULL, "&");
    int flag = 0;

    username = strtok(nameParam, "=");
    username = strtok(NULL, "=");

    second = strtok(secondParam, "=");
    if (strcmp(second, "data") == 0)
        flag++;
    
    second = strtok(NULL, "=");
    
    replce(second, '+', ' ');

    if(flag) 
        return setData(username, second);
    
    int res = Login(username, second);
    
    char* page;

    if (res == 1) 
        return getHome(username);
    

    if (res == -2) {
        page = getPage("pages/user400.html");
        page = replaceSubString(page, "%%%= msg %%%", " you got the password wrong.");
        return setRes(page, "400");
    }

    page = getPage("pages/user404.html");
    return setRes(page, "404");
}

char *getHome(char* username) {

    if (username == NULL)
        return NULL;

    char *page, *data = getData(username);
    page = getPage("pages/home.html");
    page = replaceSubString(page, "%%%= userName %%%", username);
    page = replaceSubString(page, "%%%= userName %%%", username);
    page = replaceSubString(page, "%%%= profileData %%%", data);
    if(strcmp(data, nane) != 0)
        free(data);
    return setRes(page, NULL);
}

char* getIndex() {

    char* index = getPage("pages/index.html");
    return setRes(index, NULL);
}

char* signUP (char* user) {

    if(user == NULL)
        return NULL;
    
    char* token, *nameParam = strtok(user, "&"), *passParam = strtok(NULL, "&"), *username, *pass;

    username = strtok(nameParam, "=");
    username = strtok(NULL, "=");
    pass =  strtok(passParam, "=");
    pass =  strtok(NULL, "=");
    replce(pass, '+', ' ');
    int result = Register(username, pass);
    
    char *page;

    if(result == -1) {
        page = getPage("pages/user400.html");
        page = replaceSubString(page, "%%%= msg %%%", " the user exists in the database.");
        return setRes(page, "400");
    }
    
    char* index = getPage("pages/index.html");

    return getIndex();
}