//
//  inclides.c
//  Hw11
//
//  Created by abrahem elnkeeb on 21/01/2023.
//

#include "httpd.h"
#include "includes.h"

int main(int argc, char *argv[])
{
    serve_forever("12913");
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
