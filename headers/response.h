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


char *renderHtmlResponse(const char *html);
char *renderFileResponse(const char *filepath, int *out_size);
char *renderTemplate(const char* filepath, const char** placeholders, const char** values, int count);

#endif /* response_h */