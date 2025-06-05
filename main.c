//
//  inclides.c
//  Hw11
//
//  Created by abrahem elnkeeb on 21/01/2023.
//

#include "httpd.h"
#include "handlers.h"


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

    ROUTE_POST("/view") {
        char *response = renderFileResponse(HOME_PAGE, NULL);
        if (response) {
            printf("%s", response);
            free(response);
    }
    }

    ROUTE_GET("/view/login") {
        char *response = renderFileResponse(LOGIN_PAGE, NULL);
        if (response) {
            printf("%s", response);
            free(response);
        }
    }

    ROUTE_GET_STARTS_WITH("/view/")
    {
        char *response = renderFileResponse(_404_PAGE, NULL);
        if (response) {
            printf("%s", response);
            free(response);
        }
    }

    ROUTE_POST("/api/signup") {
        fprintf(stderr, "signup 1\n");
        char *page = signUp(payload);
        printf("%s",  page);
        free(page);
    }

    ROUTE_POST("/api/signin") {
        char *page = signIn(payload);
        printf("%s",  page);
        free(page);
    }

    ROUTE_GET_STARTS_WITH("/assets/")
    {
        int response_size = 0;
        char *response = renderFileResponse(uri + 1, &response_size);
        if (response) {
            fwrite(response, 1, response_size, stdout);
            free(response);
        }
    }
  
    ROUTE_END()
}
