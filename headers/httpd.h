//
//  httpd.h
//  CServer
//
//  Created by ibrahim alankeeb on 21/01/2023.
//

#ifndef httpd_h
#define httpd_h

#include <string.h>
#include <stdio.h>

//Server control functions

void serve_forever(const char *PORT);

// Client request

extern char		*method,		// "GET" or "POST"
				*uri,			// "/index.html" things before '?'
				*qs,			// "a=1&b=2"	 things after  '?'
				*prot,			// "HTTP/1.1"
				*payload;		// for POST
extern int		payload_size;

char *request_header(const char *name);

void route();

// some interesting macro for `route()`
#define ROUTE_START()		if (0) {
#define ROUTE(METHOD,URI)	} else if (strcmp(URI,uri)==0&&strcmp(METHOD,method)==0) {
#define ROUTE_GET(URI)		ROUTE("GET", URI)
#define ROUTE_POST(URI)		ROUTE("POST", URI)
#define ROUTE_GET_STARTS_WITH(PREFIX) \
							} else if (strncmp(uri, PREFIX, strlen(PREFIX)) == 0 && strcmp(method, "GET") == 0) {

#define ROUTE_END()			} else printf(\
								"HTTP/1.1 500 Not Handled\r\n\r\n" \
								"The server has no handler to the request.\r\n" \
							);


#endif /* httpd_h */
