//
//  main.c
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
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

void route() {
	ROUTE_START()

	ROUTE_GET("/home") {
		serveHomePage(NULL);
	}

	ROUTE_GET("/login") {
		serveLoginPage();
	}

	ROUTE_GET_STARTS_WITH("/") {
		send404Page();
	}

	ROUTE_POST("/home") {
		serveHomePage(payload);
	}

	ROUTE_POST("/signup") {
		signUp(payload);
	}

	ROUTE_POST("/signin") {
		signIn(payload);
	}

	ROUTE_GET_STARTS_WITH("/public/") {
		sendFileResponse(uri + 1);
	}
  
	ROUTE_END()
}
