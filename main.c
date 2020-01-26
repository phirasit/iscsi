#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include "iscsi_connection.h"
#include "iscsi_session.h"
#include "iscsi_server.h"
#include "iscsi_pdu.h"

#include "iscsi_logger.h"

#ifndef TCP_PORT
#define TCP_PORT 4000
#endif

#define BUFFER_SIZE 256

void start_connection(struct iSCSIConnection* connection);

int main() {

  logger_line();
  logger("[MAIN] Start ISCSI server\n");

  // Create Socket
  int socket_desc;
  socket_desc = socket(AF_INET,SOCK_STREAM, 0);
  if (socket_desc == -1) {
    perror("[MAIN] Cannot create socket");
    return 1;
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(TCP_PORT);

  bind(socket_desc, (struct sockaddr *) &address, sizeof(address));
  listen(socket_desc, 32);

  logger("[MAIN] Successfully bind socket to port %d\n", TCP_PORT);
  logger_line();

  while (1)
  {
    struct sockaddr client;
    static socklen_t addr_size = sizeof(struct sockaddr_in);

    int client_socket_fd = accept(socket_desc, (struct sockaddr*) &client, &addr_size);

    /*
    if (iscsi_session_full(&iscsi_session)) {
      // session is full
      close(client_socket_fd);
      continue;
    }
    */

    // fork new process
    pid_t pid = fork();
    if (pid == -1) {
      logger("[MAIN] cannot fork a new process\n");
    } else if (pid == 0) {
      logger("[MAIN] new process is forked\n");
      struct iSCSIConnection *connection = (struct iSCSIConnection *) malloc(sizeof(struct iSCSIConnection));
      iscsi_connection_create(connection, client_socket_fd, NULL);
      start_connection(connection);
    }
  }

  return 0;
}

void* start_receiver(void* args) {
  struct iSCSIConnection* connection = (struct iSCSIConnection*) args;

  char buffer[BUFFER_SIZE];
  int len, status;

  while (1) {
    logger("[MAIN] Waiting for packet\n");
    len = recv(connection->socket_fd, buffer, BUFFER_SIZE, 0);
    if (len == -1) {
      logger("[MAIN] error occurred (code %d): %s\n", errno, strerror(errno));
      break;
    }

    logger("[MAIN] receive data length %d bytes\n", len);

    status = incoming_request(connection, buffer, len);

    logger("[MAIN] finish process with status = %d\n", status);
    logger_line();

    switch (status) {
      case SOCKET_TERMINATE: goto end; 
    }

    usleep(10000);
  }
end:
  logger("[MAIN] Connection closed\n");
  close(connection->socket_fd);
  // TODO change state to terminated
}

void* start_transmit(void* args) {
  struct iSCSIConnection* connection = (struct iSCSIConnection*) args;
  struct iSCSIBuffer* buffer = &connection->response_buffer;

  while (1) {

    // TODO break if state is terminated
    if (0) {
      break;
    }
  
    if(iscsi_pdu_valid(iscsi_buffer_data(buffer), iscsi_buffer_length(buffer))) {
      int length = iscsi_pdu_length(iscsi_buffer_data(buffer));
      logger("[MAIN] Transmit PDU of length: %d\n", length);

      iscsi_buffer_acquire_lock(buffer);
      // logger("[MAIN] PDU Data:\n");
      // logger_hex_array(iscsi_buffer_data(buffer), length);
      int result = send(connection->socket_fd, iscsi_buffer_data(buffer), length, 0);
      if (result < 0) {
        logger("[MAIN] Transmission error: %s\n", strerror(errno));
        break;
      }
      iscsi_buffer_release_lock(buffer, 0);

      iscsi_buffer_flush(buffer, length);
      logger("[MAIN] remaining in transmission buffer: %d\n", iscsi_buffer_length(buffer));
    }
    usleep(10000);
  }
  
  close (connection->socket_fd);
  logger("[MAIN] Closed transmission\n");
}

void start_connection(struct iSCSIConnection* connection) {
  pthread_t receive_thread, transmit_thread;
  int stat1 = pthread_create(&receive_thread, NULL, start_receiver, (void*) connection);
  if (stat1) {
    logger("[MAIN] Failed to start receiver thread\n");
  }
  int stat2 = pthread_create(&transmit_thread, NULL, start_transmit, (void*) connection);
  if (stat2) {
    logger("[MAIN] Failed to start transmit thread\n");
  }
  
  int result1, result2;
  pthread_join(receive_thread, (void*) &result1);
  pthread_join(transmit_thread, (void*) &result2);
}
