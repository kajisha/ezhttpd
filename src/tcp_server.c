#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <unistd.h>

#include "tcp_server.h"

static int initialize_tcp_server(TcpServer*);
static int listen_tcp_server(TcpServer*);
static int wait_on_receive_tcp_server(TcpServer*);
static void on_receive_tcp_server(const TcpServer*, int);
static void *default_tcp_server(TcpServer*, const TcpServer*);
static void *acceptor(TcpServer*);

const TcpServer DEFAULT_TCP_SERVER = {
  .initialize = initialize_tcp_server,
  .listen = listen_tcp_server,
  .wait_on_receive = wait_on_receive_tcp_server,
  .on_receive = on_receive_tcp_server
};

void *tcp_server(TcpServer* server) {
  return default_tcp_server(server, &DEFAULT_TCP_SERVER);
}

static void *default_tcp_server(TcpServer* server, const TcpServer* default_server) {
#define MERGE_ARG(member) (server->member ? server : default_server)->member
  server->port_no = MERGE_ARG(port_no);
  server->initialize = MERGE_ARG(initialize);
  server->listen = MERGE_ARG(listen);
  server->wait_on_receive = MERGE_ARG(wait_on_receive);
  server->on_receive = MERGE_ARG(on_receive);

  return acceptor(server);
#undef MERGE_ARG
}

static void *acceptor(TcpServer* server) {
  server->initialize(server);

  server->listen(server);

  printf("server listening on %d\n", server->port_no);

  for (int client_socket = -1; (client_socket = server->wait_on_receive(server));) {
    pid_t child_pid = fork();

    if (child_pid == -1) {
      perror("fork");

      break;
    } else if (child_pid == 0) {
      server->on_receive(server, client_socket);

      close(client_socket);
      exit(EXIT_SUCCESS);
    } else {
      close(client_socket);
    }
  }

  close(server->socket);

  return NULL;
}

static int initialize_tcp_server(TcpServer* server) {
  server->socket = socket(AF_INET, SOCK_STREAM, 0);

  server->addr = (struct sockaddr_in) {
    .sin_family = AF_INET,
    .sin_port = htons(server->port_no),
    .sin_addr.s_addr = INADDR_ANY
  };

  if (server->socket < 0) {
    perror("socket");

    return -1;
  }

  int status = 1;
  setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &status, sizeof(status));

  return 0;
}

static int listen_tcp_server(TcpServer* server) {
  if (bind(server->socket, (struct sockaddr *)&server->addr, sizeof(server->addr)) != 0) {
    perror("bind");

    return -1;
  }

  if (listen(server->socket, 5) != 0) {
    perror("listen");

    return -1;
  }

  return 0;
}

static int wait_on_receive_tcp_server(TcpServer* server) {
  struct sockaddr_in addr;
  int addr_len = sizeof(addr);
  int client_socket = accept(server->socket, (struct sockaddr *)&addr, (socklen_t *restrict)&addr_len);

  if (client_socket == -1) {
    perror("accept");
  }

  return client_socket;
}

static void on_receive_tcp_server(const TcpServer* server, int socket) {
}
