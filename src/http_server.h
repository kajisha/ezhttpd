#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include <stdio.h>

#include "tcp_server.h"

enum http_response_code {
  OK = 200,
  NOT_FOUND = 404,
  INTERNAL_SERVER_ERROR = 500
};

typedef struct _HttpRequest HttpRequest;
typedef struct _HttpResponse HttpResponse;

struct _HttpRequest {
  char *method;
  char *path;
  char *http_version;
  FILE *input_stream;
  FILE *output_stream;

  HttpResponse *(*read_file)(const HttpRequest *, const char *);
};

struct _HttpResponse {
  enum http_response_code status;
  char *body;

  void (*render)(const HttpRequest *, const HttpResponse *);
  const char *(*status_code)(const enum http_response_code);
  int (*content_length)(const HttpResponse *);
};

typedef struct {
  union { TcpServer tcp; };

  HttpResponse *(*handle)(const HttpRequest *request);
} HttpServer;

extern void http_server(HttpServer *);
extern HttpResponse *responder(HttpResponse *);
#endif
