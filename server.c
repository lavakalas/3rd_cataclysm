#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>

#include "connection.h"

int main (void) {
    int down_flag = 0;
    int unix_socket;
    char buffer[BUFFER_SIZE];
    int data_socket;
    int result;
    ssize_t r, w;
    struct sockaddr_un name;
    int ret;

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
    

 
    for(;;){
        data_socket = accept(unix_socket, NULL, NULL);
        if (data_socket == -1){
            perror("server_socket_accept");
            exit(1);
        }


        result = 0;
        for (;;){
            r = read(data_socket, buffer, sizeof(buffer));
            if (r == -1){
                perror("socket_server_read");
                exit(1);
            }

            buffer[sizeof(buffer) - 1] = 0;
            

            if (!strncmp(buffer, "DOWN", sizeof(buffer))){
                down_flag = 1;
                continue;
            }

            if (!strncmp(buffer, "END", sizeof(buffer))) break;
            

            if (down_flag) continue;

            result += atoi(buffer);
        }

        sprintf(buffer, "%d", result);

        w = write(data_socket, buffer, sizeof(buffer));
        if (w == -1){
            perror("server_socket_write");
            exit(1);
        }


        close(data_socket);
        if (down_flag) break;
    }

    unlink(SOCKET_NAME);
    exit(0);

}
