//
//  response.h
//  CServer
//
//  Created by ibrahim alankeeb on 21/01/2023.
//

#ifndef response_h
#define response_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "pages.h"

#define MIME_HTML	"text/html"
#define MIME_CSS	"text/css"
#define MIME_PLAIN	"text/plain"
#define MIME_JS		"application/javascript"
#define MIME_ICO	"image/x-icon"
#define MIME_PNG	"image/png"
#define MIME_JPEG	"image/jpeg"
#define MIME_BIN	"application/octet-stream"

#define STATUS_200_OK				"HTTP/1.1 200 OK"
#define STATUS_302_FOUND			"HTTP/1.1 302 Found"
#define STATUS_400_BAD_REQUEST		"HTTP/1.1 400 Bad Request"
#define STATUS_401_UNAUTHORIZED		"HTTP/1.1 401 Unauthorized"
#define STATUS_403_FORBIDDEN		"HTTP/1.1 403 Forbidden"
#define STATUS_404_NOT_FOUND		"HTTP/1.1 404 Not Found"
#define STATUS_500_INTERNAL_ERROR	"HTTP/1.1 500 Internal Server Error"

#define GET_FILE(path) \
	getFile(path, NULL)

#define READ_FILE_WITH_SIZE(path, outSizePtr) \
	getFile(path, outSizePtr)

#define RENDER_FILE(path) \
	renderFileResponse(path, NULL)

#define RENDER_FILE_WITH_SIZE(path, outSizePtr) \
	renderFileResponse(path, outSizePtr)

#define REDIRECT(location) \
	redirectInternal(location, STATUS_302_FOUND, 0)

#define REDIRECT_CLEAR(location) \
	redirectInternal(location, STATUS_302_FOUND, 1)

char *renderErrorPage(const char *status, const char *message);
char *renderHtmlResponse(const char *html, const char *status);
char *renderFileResponse(const char *filepath, int *out_size);
char *renderTemplate(const char *filepath, const char **placeholders, const char **values, int count);

#endif /* response_h */