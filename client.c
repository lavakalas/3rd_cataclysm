#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "connection.h"

int main(int argc, char *argv[]) {
    int ret;
    int data_socket;
    ssize_t r, w;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];


    data_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);

    if (data_socket == -1){
        perror("clien_socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    ret = connect(data_socket, (const struct sockaddr *) &addr, sizeof(addr));

    if (ret == -1){
        perror("client_socket_connect");
        exit(1);
    }

    for (int i = 1; i < argc; ++i){
        w = write(data_socket, argv[i], sizeof(argv[i]) + 1);
        if (w == -1){
            perror("clien_socket_write");
            break;
        }
    }

    strcpy(buffer, "END");
    w = write(data_socket, buffer, strlen(buffer) + 1);
    if (w == -1){
        perror("clien_socket_write");
        exit(1);
    }
    
    r = read(data_socket, buffer, sizeof(buffer));

    if (r == -1){
        perror("client_socket_read");
        exit(1);
    }

    buffer[sizeof(buffer) - 1] = 0;
    
    printf("Server response is %s\n", buffer);
    
    close(data_socket);
    exit(0);
}
