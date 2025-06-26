//
//  response.c
//  CServer
//
//  Created by ibrahim alnakeeb on 21/01/2023.
//

#include "response.h"

const char *get_mime_type(const char *path) {
	assert(path != NULL);

	const char *ext = strrchr(path, '.');
	if (!ext) return MIME_BIN;

	if (strcmp(ext, ".html") == 0)	return MIME_HTML;
	if (strcmp(ext, ".css") == 0)	return MIME_CSS;
	if (strcmp(ext, ".js") == 0)	return MIME_JS;
	if (strcmp(ext, ".ico") == 0)	return MIME_ICO;
	if (strcmp(ext, ".png") == 0) 	return MIME_PNG;
	if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return MIME_JPEG;

	return MIME_BIN;
}

// Allocates memory, caller must free result
char *getFile(const char *path, int *out_size) {
	assert(path != NULL);

	FILE *file = fopen(path, "rb");
	if (!file) return NULL;

	// Seek to the end to determine file size
	if (fseek(file, 0, SEEK_END) != 0) {
		fclose(file);
		return NULL;
	}

	long size = ftell(file);
	if (size < 0) {
		fclose(file);
		return NULL;
	}
	rewind(file);

	// Allocate exact size (no null terminator)
	char *buffer = malloc(size);
	if (!buffer) {
		fclose(file);
		return NULL;
	}

	size_t bytes_read = fread(buffer, 1, size, file);
	fclose(file);

	if (bytes_read != (size_t)size) {
		free(buffer);
		return NULL;
	}

	if(out_size) {
		*out_size = (int)bytes_read;
	}

	return buffer;
}

// Allocates memory, caller must free result
char *renderTemplate(const char *filepath, const char **placeholders, const char **values, int count) {
	assert((count == 0) || (placeholders && values));

	char *page = GET_FILE(filepath);

	for (int i = 0; i < count; i++) {
		const char *key = placeholders[i];
		const char *value = values[i];

		char *temp = NULL;

		// Replace all occurrences of key with value
		while (1) {
			char *pos = strstr(page, key);
			if (!pos) break;

			size_t before_len = pos - page;
			size_t after_len = strlen(pos + strlen(key));
			size_t new_len = before_len + strlen(value) + after_len;

			temp = malloc(new_len + 1);
			strncpy(temp, page, before_len);
			strcpy(temp + before_len, value);
			strcpy(temp + before_len + strlen(value), pos + strlen(key));

			free(page);
			page = temp;
		}
	}
	return page;
}

// Allocates memory, caller must free result
char *renderFileResponse(const char *filepath, int *out_size) {
	assert(filepath != NULL);

	if (strncmp(filepath, "assets/db/", 10) == 0) {
		const char *response_str =
			"HTTP/1.1 403 Forbidden\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: 13\r\n"
			"Connection: close\r\n"
			"\r\n"
			"403 Forbidden";

		if (out_size) 
			*out_size = strlen(response_str);
		return strdup(response_str);
	}


	int file_size = 0;
	char *content = getFile(filepath, &file_size);
	const char *mime_type = get_mime_type(filepath);
	const char *status_line = "HTTP/1.1 200 OK";

	if (!content) {
		fprintf(stderr, "file %s not found\n", filepath);

		if (strcmp(mime_type, MIME_HTML) == 0) {
			content = getFile(_404_PAGE, &file_size);
			if (content) {
				mime_type = MIME_HTML;
				status_line = "HTTP/1.1 404 Not Found";
			}
		}

		if (!content) {
			mime_type = MIME_PLAIN;
			content = strdup("404 Not Found");
			file_size = strlen(content);
			status_line = "HTTP/1.1 404 Not Found";
		}
	}

	// Allocate memory for the full HTTP response
	int header_size = snprintf(NULL, 0,
		"%s\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %d\r\n"
		"Connection: close\r\n"
		"\r\n",
		status_line, mime_type, file_size
	);

	char *response = malloc(header_size + file_size + 1);
	if (!response) {
		free(content);
		return NULL;
	}

	snprintf(response, header_size + 1,
		"%s\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %d\r\n"
		"Connection: close\r\n"
		"\r\n",
		status_line, mime_type, file_size
	);

	memcpy(response + header_size, content, file_size);
	response[header_size + file_size] = '\0';

	free(content);
	if (out_size)
		*out_size = header_size + file_size;

	return response;
}

// Allocates memory, caller must free result
char *renderHtmlResponse(const char *html, const char *status) {
	int body_len = strlen(html);
	const char *type = "text/html";

	int header_len = snprintf(NULL, 0,
		"%s\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n",
		status, type, body_len);

	char *response = malloc(header_len + body_len + 1);
	if (!response) return NULL;

	snprintf(response, header_len + 1,
		"%s\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n",
		status, type, body_len);

	memcpy(response + header_len, html, body_len);
	response[header_len + body_len] = '\0';
	return response;
}

void renderErrorPage(const char *message) {
	const char *placeholders[] = { "{{message}}" };
	const char *values[] = { message };

	char *rendered_html = renderTemplate(ERROR_PAGE, placeholders, values, 1);
	if (!rendered_html) {
		sendFallback500Response();
		return;
	}

	char *response = renderHtmlResponse(rendered_html, STATUS_500_INTERNAL_ERROR);
	free(rendered_html);

	printf("%s", response);
	free(response);
}

void redirect(const char *location, const char *status, int clearCookie, const char *sessionToken) {
	char cookieHeader[BUFFER_SIZE] = "";

	if (clearCookie) {
		strcpy(cookieHeader, "Set-Cookie: session=deleted; Max-Age=0; Path=/; HttpOnly; SameSite=Strict\r\n");
	} else if (sessionToken && *sessionToken) {
		snprintf(cookieHeader, sizeof(cookieHeader),
			"Set-Cookie: session=%s; Max-Age=3600; Path=/; HttpOnly; SameSite=Strict\r\n",
			sessionToken
		);
	}

	printf(
		"%s\r\n"
		"Location: %s\r\n"
		"%s"
		"Content-Length: 0\r\n"
		"Connection: close\r\n"
		"\r\n",
		status, location, cookieHeader
	);
}

void sendFallback500Response() {
	const char *message = "An unexpected error occurred. Please try again later.\r\n";
	printf(
		"%s\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %zu\r\n"
		"Connection: close\r\n"
		"\r\n"
		"%s", 
		STATUS_500_INTERNAL_ERROR, MIME_PLAIN, strlen(message), message
	);
}
