#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "iscsi.h"
#include "iscsi_session.h"
#include "iscsi_server.h"
#include "iscsi_connection.h"

#ifndef TCP_PORT
#define TCP_PORT 4000
#endif

#define BUFFER_SIZE 256

void start_connection(struct iSCSIConnection* connection);

int main() {

  // Create Socket
  int socket_desc;
  socket_desc = socket(AF_INET,SOCK_STREAM, 0);
  if (socket_desc == -1)
    perror("Cannot create socket");

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(TCP_PORT);

  bind(socket_desc, (struct sockaddr *) &address, sizeof(address));
  listen(socket_desc, 32);

  fprintf(stderr, "Successfully bind socket to port %d\n", TCP_PORT);

  // Create socket
  struct iSCSISession iscsi_session;
  iscsi_session_create(&iscsi_session);

  while (1)
  {
    struct sockaddr client;
    static socklen_t addr_size = sizeof(struct sockaddr_in);

    int client_socket_fd = accept(socket_desc, (struct sockaddr*) &client, &addr_size);

    if (iscsi_session_full(&iscsi_session)) {
      // session is full
      close(client_socket_fd);
      continue;
    }

    // TODO fix this
    struct iSCSIConnection connection;
    iscsi_connection_create(&connection, client_socket_fd, &iscsi_session);
    start_connection(&connection);
  }

  return 0;
}

void start_connection(struct iSCSIConnection* connection) {
  char buffer[BUFFER_SIZE];
  int len, status;

  while (1) {
    len = recv(connection->socket_fd, buffer, BUFFER_SIZE, 0);
    if (len == 0) break; // connection close

    status = incoming_request(connection, buffer, len);
    sleep(100);

    if (status == SOCKET_TERMINATE) {
      break;
    }
  }

  close(connection->socket_fd);
}
