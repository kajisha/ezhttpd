#include <stdio.h>
#include "tcp_server.h"

int main() {

  tcp_server(&(TcpServer) {
    .port_no = 8000
  });

  return 0;
}
