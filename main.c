//
//  inclides.c
//  Hw11
//
//  Created by abrahem elnkeeb on 21/01/2023.
//

#include "httpd.h"
#include "includes.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    setUp();
    const char *port = argv[1];
    serve_forever(port);
    return 0;
}

void route()
{
    ROUTE_START()

    ROUTE_GET("/")
    {
        char* responst = getIndex();
        printf("%s", responst);
        free(responst);
    }


    ROUTE_POST("/home")
    {
        char *page = loadHome(payload);
        printf("%s",  page);
        free(page);
    }

    ROUTE_POST("/register") 
    {
        char *page = signUP(payload);
        printf("%s",  page);
        free(page);
    }
  
    ROUTE_END()
}
