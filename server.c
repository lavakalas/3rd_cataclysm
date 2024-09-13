#include <stdio.h>
#include <sys/signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>

#include "connection.h"

void handler(int signum){
    printf("Server recieved SIGUSR1 from client\n");
    unlink(SOCKET_NAME);
    exit(0);
}

int main (void) {
    int down_flag = 0;
    int unix_socket;
    char buffer[BUFFER_SIZE];
    int data_socket;
    int result;
    ssize_t r, w;
    struct sockaddr_un name;
    int ret;
    int pid;

    unix_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (unix_socket == -1){
        perror("server_socket");
        exit(1);
    }
    
    memset(&name, 0, sizeof(name));

    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
    
    ret = bind(unix_socket, (const struct sockaddr *) &name, sizeof(name));

    if (ret == -1){
        perror("server_socket_bind");
        exit(1);
    }

    ret = listen(unix_socket, 15);

    if (ret == -1){
        perror("server_socket_listen");
        exit(1);
    }
    
    data_socket = accept(unix_socket, NULL, NULL);
    if (data_socket == -1){
        perror("server_socket_accept");
        exit(1);
    }


    pid = getpid();
    sprintf(buffer, "%d", pid);
    w = write(data_socket, buffer, sizeof(buffer));
    if (w == -1){
        perror("server_socket_write");
        exit(1);
    }

    if (signal(SIGUSR1, handler) == SIG_ERR){
        perror("server_signal_handler");
        exit(1);
    }
 
    for(;;){
        r = read(data_socket, buffer, sizeof(buffer));
        if (r == -1){
            perror("server_socker_read");
            exit(1);
        }

        if (!strncmp(buffer, "CAPTURED PID", sizeof(buffer))){
           printf("\n\033[0;33mClient acquired server PID, closing data_socket\033[0m\n");
           close(data_socket);
           break;
        }
    }

    for(;;){
        sleep(1);
    }
    unlink(SOCKET_NAME);
    exit(0);

}
