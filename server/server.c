#include <stdio.h>          // printf, perror
#include <stdlib.h>         // exit, EXIT_FAILURE
#include <string.h>         // memset, strlen
#include <unistd.h>         // close
#include <arpa/inet.h>      // inet_ntoa, htons, sockaddr_in
#include <sys/socket.h>     // socket, bind, listen, accept, recv, send
#include <pthread.h>

struct MovementPacket {
    int type;
    float x;
    float y;
};

void *handle_client(void *arg){

    int client_fd = *((int *) arg);
    free(arg);
    int bytes_received;
    struct MovementPacket move;
    
    while((bytes_received = recv(client_fd, &move, sizeof(move), 0)) > 0){
        if(bytes_received != sizeof(move)){
            printf("Partial packet received\n");
        }

        if (move.type == 1) {
            printf("Client %d moved to (%.2f, %.2f)\n", client_fd, move.x, move.y);
        } else {
            printf("Unknown packet type: %d\n", move.type);
        }
    }
    if (bytes_received < 0) {
        perror("recv");
    }
    close(client_fd);
    pthread_exit(NULL);

}

int main() {
    int server_fd;
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);

    }
    printf("Socket created and bound to port 8080.\n");
    if(listen(server_fd, 4)<0){
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening...\n");
    while(1){
        struct sockaddr_in client_addr; 
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        // printf("Client connected\n");
        if(client_fd < 0){
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_fd_ptr);
        pthread_detach(tid);
    }

    close(server_fd);
    

    return 0;
}