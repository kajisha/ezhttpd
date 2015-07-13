#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <stdlib.h>
#include <stdbool.h>

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <unistd.h>

typedef struct _TcpServer TcpServer;

struct _TcpServer {
  uint16_t port_no;
  struct sockaddr_in addr;
  int socket;

  int (*initialize)(TcpServer *);
  int (*listen)(TcpServer *);
  int (*wait_on_receive)(TcpServer *);
  void (*on_receive)(const TcpServer *, int);
};

extern void *tcp_server(TcpServer *);
#endif
