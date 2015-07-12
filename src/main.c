#include <stdio.h>

#include "http_server.h"

HttpResponse *handler(const HttpRequest *request) {
  printf("HTTP version -> %s\n", request->http_version);
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
