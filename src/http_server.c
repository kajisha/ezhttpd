#include <stdio.h>
#include <string.h>

#include "tcp_server.h"
#include "http_server.h"

static void on_receive(const TcpServer*, int);
static void process_http_request(HttpServer*, HttpRequest*);
static void read_http_request(HttpRequest*);
static void write_http_response(HttpRequest*, HttpResponse*);
static void destroy_http_request(HttpRequest *);
static char *chomp(char *);

void http_server(HttpServer* server) {
  server->tcp.on_receive = on_receive;

  tcp_server((TcpServer *)server);
}

static char *chomp(char *str) {
  char *head = str;
  while (*++str != '\r' || *++str != '\n');
  *str = '\0';

  return head;
}

static void on_receive(const TcpServer* tcp, int socket) {
  process_http_request((HttpServer* )tcp, &(HttpRequest) {
    .input_stream = fdopen(socket, "r+"),
    .output_stream = fdopen(socket, "w")
  });
}

static void process_http_request(HttpServer *server, HttpRequest *request) {
  read_http_request(request);

  HttpResponse *response = server->handle(request);

  write_http_response(request, response);

  destroy_http_request(request);
}

static void read_http_request(HttpRequest *request) {
  char buf[BUFSIZ];

  if (fgets(buf, sizeof(buf), request->input_stream)) {
    if (strncmp(buf, "GET", 3) == 0) {
      char *token = strtok(buf, " ");

      request->method = strdup(token);

      token = strtok(NULL, " ");
      request->path = strdup(token);

      token = strtok(NULL, " ");
      request->http_version = strdup(chomp(token));
    }
  }
}

static void write_http_response(HttpRequest *request, HttpResponse *response) {
  const char *content = "<html><body>hello</body></html>\r\n\r\n";

  fprintf(request->output_stream, "HTTP/1.1 200 OK\r\n");
  fprintf(request->output_stream, "Server: ezhttpd\r\n");
  fprintf(request->output_stream, "Date: Sun, 12 Jul 2015 07:45:46 GMT\r\n");
  fprintf(request->output_stream, "Content-Type: text/html; charset=UTF-8\r\n");
  fprintf(request->output_stream, "Connection: close\r\n");
  fprintf(request->output_stream, "Content-Length: %d\r\n", (int) strlen(content));
  fprintf(request->output_stream, "\r\n");
  fprintf(request->output_stream, content);

  fflush(request->output_stream);
}

static void destroy_http_request(HttpRequest *request) {
  free(request->method);
  free(request->path);
  free(request->http_version);

  fclose(request->input_stream);
  fclose(request->output_stream);
}
