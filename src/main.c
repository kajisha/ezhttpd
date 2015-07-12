#include <stdio.h>

#include "http_server.h"

extern HttpResponse *handler(const HttpRequest*);

HttpResponse *handler(const HttpRequest *request) {
  printf("HTTP method -> %s\n", request->method);
  printf("HTTP path -> %s\n", request->path);

  return NULL;
}

int main() {
  http_server(&(HttpServer) {
    .tcp.port_no = 8000,
    .handle = handler
  });

  return 0;
}
