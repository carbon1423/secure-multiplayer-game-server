#include <stdio.h>          // printf, perror
#include <stdlib.h>         // exit, EXIT_FAILURE
#include <string.h>         // memset, strlen
#include <unistd.h>         // close
#include <arpa/inet.h>      // inet_ntoa, htons, sockaddr_in
#include <sys/socket.h>     // socket, bind, listen, accept, recv, send
#include <pthread.h>
#include <math.h>
#include "../shared/packet.h"
#include "../shared/gameconfig.h"
#include "../shared/tilemap.h"





pthread_mutex_t players_lock = PTHREAD_MUTEX_INITIALIZER;
Player players[MAX_CLIENTS];
Bullet bullets[MAX_CLIENTS];
InputPacket latest_inputs[MAX_CLIENTS];

void *handle_client(void *arg) {
    ClientArgs *args = (ClientArgs *)arg;
    int client_fd = args->client_fd;
    int player_id = args->player_id;
    free(arg);
    
    InputPacket input;
    ssize_t bytes_received;
    
    while ((bytes_received = recv(client_fd, &input, sizeof(input), 0)) > 0) {
        if (bytes_received == sizeof(input)) {
            pthread_mutex_lock(&players_lock);
            latest_inputs[player_id] = input;
            pthread_mutex_unlock(&players_lock);
        }
    }
    
    // Cleanup on disconnect
    pthread_mutex_lock(&players_lock);
    players[player_id].active = 0;
    players[player_id].client_fd = -1;
    bullets[player_id].still_render = 0;
    pthread_mutex_unlock(&players_lock);
    
    close(client_fd); 
    pthread_exit(NULL);
}

void *broadcast_thread(void *arg){
    while (1) {
        BroadcastPacket packet;
        packet.count = 0;

        pthread_mutex_lock(&players_lock);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!players[i].active) continue;

            InputPacket input = latest_inputs[i];

            // ===== Movement =====
            float ax = 0;
            if (input.type & INPUT_MOVE) {
                if (input.left) ax -= ACCEL;
                if (input.right) ax += ACCEL;
            }

            players[i].vx += ax;
            if (ax == 0) players[i].vx *= FRICTION;
            if (fabs(players[i].vx) < 0.05f) players[i].vx = 0;
            if (players[i].vx > MAX_SPEED) players[i].vx = MAX_SPEED;
            if (players[i].vx < -MAX_SPEED) players[i].vx = -MAX_SPEED;
            players[i].x += players[i].vx;

            // ===== Jump =====
            if (!players[i].on_ground) players[i].vy += 0.7;
            players[i].y += players[i].vy;
            int foot_y = players[i].y + PLAYER_HEIGHT;
            int center_x = players[i].x + PLAYER_WIDTH / 2;

            int tile_row = foot_y / TILE_SIZE;
            int tile_col = center_x / TILE_SIZE;

            if (tile_row >= 0 && tile_row < MAP_HEIGHT &&
                tile_col >= 0 && tile_col < MAP_WIDTH &&
                tilemap[tile_row][tile_col].type == TILE_SOLID) {

                // Snap player to top of tile
                players[i].y = tile_row * TILE_SIZE - PLAYER_HEIGHT;
                players[i].vy = 0;
                players[i].on_ground = 1;
            } else {
                players[i].on_ground = 0;
            }

            // if (players[i].y >= WIN_HEIGHT - PLAYER_HEIGHT) {
            //     players[i].y = WIN_HEIGHT - PLAYER_HEIGHT;
            //     players[i].vy = 0;
            //     players[i].on_ground = 1;
            // } else {
            //     players[i].on_ground = 0;
            // }

            if ((input.type & INPUT_JUMP) && players[i].on_ground) {
                players[i].vy = -15;
                players[i].on_ground = 0;
            }

            // ===== Shooting =====
            if ((input.type & INPUT_SHOOT) && !bullets[i].still_render) {
                float dx = input.mouseX - players[i].x;
                float dy = input.mouseY - players[i].y;
                float len = sqrtf(dx * dx + dy * dy);
                if (len != 0) {
                    dx /= len;
                    dy /= len;
                }
                bullets[i].x = players[i].x;
                bullets[i].y = players[i].y;
                bullets[i].vx = dx * 20.0f;
                bullets[i].vy = dy * 20.0f;
                bullets[i].still_render = 1;
            }

            // ===== Bullet Movement =====
            if (bullets[i].still_render) {
                bullets[i].x += bullets[i].vx;
                bullets[i].y += bullets[i].vy;
                for(int j = 0; j < MAX_CLIENTS; j ++){
                    if(j != i && players[j].active && bullets[i].x > players[j].x && bullets[i].x < players[j].x + PLAYER_WIDTH && bullets[i].y > players[j].y && bullets[i].y < players[j].y + PLAYER_HEIGHT){
                        players[j].health -= 1;
                        bullets[i].still_render = 0;
                    }
                }
                if (bullets[i].x < 0 || bullets[i].x > WIN_LENGTH ||
                    bullets[i].y < 0 || bullets[i].y > WIN_HEIGHT) {
                    bullets[i].still_render = 0;
                }
            }

            // ===== Fill Packet =====
            packet.players[packet.count].id = i;
            packet.players[packet.count].x = players[i].x;
            packet.players[packet.count].y = players[i].y;
            packet.players[packet.count].remaining_health = players[i].health;
            packet.bullets[packet.count].x = bullets[i].x;
            packet.bullets[packet.count].y = bullets[i].y;
            packet.bullets[packet.count].still_render = bullets[i].still_render;
            packet.count++;
        }

        pthread_mutex_unlock(&players_lock);

        // Broadcast to all clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (players[i].active) {
                ssize_t sent = send(players[i].client_fd, &packet, sizeof(packet), 0);
                if (sent < 0) perror("send");
            }
        }

        usleep(1000000 / 60); // 60 FPS
    }
    return arg;
}

int main() {
    generate_test_map();
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
                players[i].x = i * 50;
                players[i].y = 0;
                players[i].health = 3;
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