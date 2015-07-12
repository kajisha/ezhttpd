#include <stdio.h>
#include <string.h>

#include "tcp_server.h"
#include "http_server.h"

static void on_receive(const TcpServer*, int);
static void process_http_request(HttpServer*, HttpRequest*);
static void read_http_request(HttpRequest*);
static void write_http_response(HttpRequest*, HttpResponse*);

void http_server(HttpServer* server) {
  server->tcp.on_receive = on_receive;

  tcp_server((TcpServer *)server);
}

static void on_receive(const TcpServer* tcp, int socket) {
  process_http_request((HttpServer* )tcp, &(HttpRequest) {
    .in = fdopen(socket, "r"),
    .out = fdopen(socket, "w")
  });
}

static void process_http_request(HttpServer *server, HttpRequest *request) {
  read_http_request(request);

  HttpResponse *response = server->handle(request);

  write_http_response(request, response);

  fclose(request->in);
  fclose(request->out);
}

static void read_http_request(HttpRequest *request) {
  char buf[BUFSIZ];

  if (fgets(buf, sizeof(buf), request->in)) {

    if (strncmp(buf, "GET", 3) == 0) {
      request->method = "GET";

      // parse path
      char *start_path = strchr(buf, ' ');
      char *end_path = strchr(++start_path, ' ');
      *end_path = '\0';

      request->path = strdup(start_path);
    }
  }
}

static void write_http_response(HttpRequest *request, HttpResponse *response) {
  fprintf(request->out, "HTTP/1.1 200 OK\r\n");
  fprintf(request->out, "Server: ezhttpd\r\n");
  fprintf(request->out, "Date: Sun, 12 Jul 2015 07:45:46 GMT\r\n");
  fprintf(request->out, "Content-Type: text/html\r\n");
  fprintf(request->out, "Content-Length: 30\r\n");
  fprintf(request->out, "Connection: close\r\n");
  fprintf(request->out, "\r\n");
  fprintf(request->out, "<html><body>hello</body></html>\r\n");
}
