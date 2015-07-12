#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include <stdio.h>

#include "tcp_server.h"

typedef struct HttpRequest {
  char *method;
  char *path;
  char *http_version;
  FILE *stream;
} HttpRequest;

typedef struct HttpResponse {
  char *body;
} HttpResponse;

typedef struct HttpServer {
  union { TcpServer tcp; };

  HttpResponse *(*handle)(const HttpRequest *request);
} HttpServer;

extern void http_server(HttpServer*);
#endif
