#include <stdio.h>
#include <string.h>

#include "tcp_server.h"
#include "http_server.h"

static void on_receive(const TcpServer*, int);
static void process_http_request(HttpServer *, HttpRequest *);
static void read_http_request(HttpRequest*);
static void write_http_response(const HttpRequest *, const HttpResponse *);
static void destroy_http_request(HttpRequest *);

static void http_response_renderer(const HttpRequest *, const HttpResponse *);
static int http_response_content_length(const HttpResponse *);
static const char *http_response_status_code(const enum http_response_code);
static void destroy_http_response(HttpResponse *);

static char *chomp(char *);

void http_server(HttpServer* server) {
  server->tcp.on_receive = on_receive;

  tcp_server((TcpServer *)server);
}

const HttpResponse DEFAULT_HTTP_RESPONDER = {
  .render = http_response_renderer,
  .status_code = http_response_status_code,
  .content_length = http_response_content_length
};

HttpResponse *responder(HttpResponse *response) {
#define MERGE_ARG(member) (response->member ? response : &DEFAULT_HTTP_RESPONDER)->member
  return &(HttpResponse) {
    .status = MERGE_ARG(status),
    .body = MERGE_ARG(body),
    .render = MERGE_ARG(render),
    .status_code = MERGE_ARG(status_code),
    .content_length = MERGE_ARG(content_length)
  };
#undef MERGE_ARG
}

static inline const char *http_response_status_code(const enum http_response_code status) {
  switch (status) {
    case OK:
      return "OK";
    case NOT_FOUND:
      return "Not Found";
    case INTERNAL_SERVER_ERROR:
      return "Internal Server error";
  }

  // not reached
  return NULL;
}

static inline void http_response_renderer(const HttpRequest *request, const HttpResponse *response) {
  fprintf(request->output_stream, "%s %d %s\r\n", request->http_version, response->status, response->status_code(response->status));
  fprintf(request->output_stream, "Content-Type: text/html; charset=""UTF-8""");
  if (response->body) {
    fprintf(request->output_stream, "Content-Length: %d\r\n", response->content_length(response));
  }
  fprintf(request->output_stream, "\r\n");

  if (response->body) {
    fprintf(request->output_stream, "%s\r\n", response->body);
    fprintf(request->output_stream, "\r\n");
  }
  fflush(request->output_stream);
}

static char *chomp(char *str) {
  char *head = str;
  for (; *str != '\r'; str++);
  *str = '\0';

  return head;
}

static void on_receive(const TcpServer* tcp, int socket) {
  process_http_request((HttpServer* )tcp, &(HttpRequest) {
    .input_stream = fdopen(socket, "r"),
    .output_stream = fdopen(socket, "w")
  });
}

static void process_http_request(HttpServer *server, HttpRequest *request) {
  read_http_request(request);

  HttpResponse *response = server->handle(request);

  write_http_response(request, response);

  destroy_http_request(request);
  destroy_http_response(response);
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

static int http_response_content_length(const HttpResponse *self) {
  return (int) strlen(self->body);
}

static void write_http_response(const HttpRequest *request, const HttpResponse *response) {
  response->render(request, response);
}

static void destroy_http_request(HttpRequest *request) {
  free(request->method);
  free(request->path);
  free(request->http_version);

  fclose(request->input_stream);
  fclose(request->output_stream);
}

static void destroy_http_response(HttpResponse *response) {
  free(response->body);
}
