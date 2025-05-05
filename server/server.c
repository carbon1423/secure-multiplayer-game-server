#include <stdio.h>          // printf, perror
#include <stdlib.h>         // exit, EXIT_FAILURE
#include <string.h>         // memset, strlen
#include <unistd.h>         // close
#include <arpa/inet.h>      // inet_ntoa, htons, sockaddr_in
#include <sys/socket.h>     // socket, bind, listen, accept, recv, send
#include <pthread.h>
#include "../packet.h"




pthread_mutex_t players_lock = PTHREAD_MUTEX_INITIALIZER;
Player players[MAX_CLIENTS];


void *handle_client(void *arg){
    ClientArgs *args = (ClientArgs *)arg;
    int client_fd = args->client_fd;
    int player_id = args->player_id;
    free(arg);
    int bytes_received;
    struct MovementPacket move;
    
    while((bytes_received = recv(client_fd, &move, sizeof(move), 0)) > 0){
        if(bytes_received != sizeof(move)){
            printf("Partial packet received\n");
        }

        if (move.type == 1) {
            printf("Client %d moved to (%.2f, %.2f)\n", client_fd, move.x, move.y);
            pthread_mutex_lock(&players_lock);
            players[player_id].x = move.x;
            players[player_id].y = move.y;
            pthread_mutex_unlock(&players_lock);
        } else {
            printf("Unknown packet type: %d\n", move.type);
        }
    }
    if (bytes_received < 0) {
        perror("recv");
    }
    close(client_fd);
    players[player_id].active = 0;
    players[player_id].client_fd = -1;
    players[player_id].x = 0;
    players[player_id].y = 0;
    pthread_exit(NULL);

}

void *broadcast_thread(void *arg){
    while (1){
        struct BroadcastPacket packet;
        packet.count = 0;

        pthread_mutex_lock(&players_lock);
        for(int i = 0;i < MAX_CLIENTS; i++){
            if(players[i].active){
                packet.players[packet.count].x = players[i].x;
                packet.players[packet.count].y = players[i].y;
                packet.count++;
            }
        }
        pthread_mutex_unlock(&players_lock);

        for(int i = 0; i < MAX_CLIENTS; i ++){
            if(players[i].active) {
                ssize_t sent = send(players[i].client_fd, &packet, sizeof(packet), 0);
                if (sent < 0) {
                    perror("send");
                }
            }
        }

        usleep(33333);
    }
    return arg;
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
    pthread_t broadcast_tid;
    pthread_create(&broadcast_tid,NULL,broadcast_thread,NULL);
    pthread_detach(broadcast_tid);
    while(1){
        struct sockaddr_in client_addr; 
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if(client_fd < 0){
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }        

        int player_id = -1;
        pthread_mutex_lock(&players_lock);
        for (int i = 0; i < MAX_CLIENTS; i++){
            if(!players[i].active){
                player_id = i;
                players[i].active = 1;
                players[i].client_fd = client_fd;
                players[i].x = 0;
                players[i].y = 0;
                int sent = send(client_fd, &player_id, sizeof(player_id), 0);
                if(sent < 0){
                    perror("send");
                    close(server_fd);
                    exit(EXIT_FAILURE);
                }
                break;
            }
        }
        pthread_mutex_unlock(&players_lock);

        if (player_id == -1) {
            printf("Server full\n");
            close(client_fd);
            continue;
        }

        
        ClientArgs *client_args = malloc(sizeof(ClientArgs));
        client_args->client_fd = client_fd;
        client_args->player_id = player_id;

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_args);
        pthread_detach(tid);
    }

    close(server_fd);
    

    return 0;
}