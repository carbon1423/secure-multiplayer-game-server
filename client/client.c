#include <stdio.h>            // printf, perror
#include <stdlib.h>           // exit, malloc
#include <string.h>           // memset
#include <unistd.h>           // close
#include <arpa/inet.h>        // sockaddr_in, inet_pton, htons
#include <sys/socket.h>       // socket, connect, send
#include <SDL2/SDL.h>         // SDL2 main headers
#include "../packet.h"


int main() {
    const int FPS = 60;
    const Uint32 frameDelay = 1000 / FPS;  // ~16 ms
    // float vy = 0;
    // float vx = 0;
    // float ax = 0;
    // const float accel = 1.0f;
    // const float max_speed = 10.0f;
    // const float friction = 0.95f;
    // int on_ground = 0;
    // float gravity = 0.5;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("SDL2 Client",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    // SDL_Rect player = {100, 100, 50, 50};
    int running = 1;
    SDL_Event event;

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    int player_id;
    int bytes_received = recv(sock_fd, &player_id, sizeof(player_id), 0);
    if(bytes_received < 0){
        perror("recv");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    while (running) {
        Uint32 frameStart = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }
        //     if (event.type == SDL_KEYDOWN) {
        //         if (event.key.keysym.sym == SDLK_w && on_ground) {
        //             vy -= 10;
        //             on_ground = 0;
        //         } else if (event.key.keysym.sym == SDLK_s) {
        //             vy += 10;
        //         } 

                
        //     }
        // }
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        InputPacket input = {
            .type = 1,
            .left = keys[SDL_SCANCODE_A],
            .right = keys[SDL_SCANCODE_D],
            .jump = (keys[SDL_SCANCODE_W])
        };

        ssize_t sent = send(sock_fd, &input, sizeof(input), 0);
        if(sent < 0){
            perror("send");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }


        // ax = 0;

        // if (keys[SDL_SCANCODE_A]) {
        //     ax = -accel;
        // } else if (keys[SDL_SCANCODE_D]) {
        //     ax = accel;
        // }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
        SDL_RenderClear(renderer);
        struct BroadcastPacket broad_pack;
        int bytes_recv = recv(sock_fd, &broad_pack, sizeof(broad_pack), 0);
        SDL_Rect other;
        other.w = 50;
        other.h = 50;

        // if (!on_ground) {
        //     vy += gravity;     // gravity pulls down
        //     player.y += vy;    // update position
        // }
        // if (player.y >= 600-player.h) {
        //     player.y = 600-player.h;
        //     vy = 0;
        //     on_ground = 1;
        // }
        // vx += ax;

        // // Apply friction when no horizontal input
        // if (ax == 0) {
        //     vx *= friction;
        //     if (fabs(vx) < 0.1f) vx = 0;
        // }

        // // Clamp horizontal speed
        // if (vx > max_speed) {
        //     vx = max_speed;
        // }
        // if (vx < -max_speed) {
        //     vx = -max_speed;
        // }

        // // Move player
        // player.x += vx;

        // struct MovementPacket packet;
        // packet.type = 1;
        // packet.x = player.x;
        // packet.y = player.y;
        // ssize_t sent = send(sock_fd, &packet, sizeof(packet), 0);
        // if (sent < 0) {
        //     perror("send");
        // }



        if(bytes_recv > 0){
            for(int i = 0; i < broad_pack.count; i++){
                if(player_id != i){
                    other.x = broad_pack.players[i].x;
                    other.y = broad_pack.players[i].y;
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // blue player
                    SDL_RenderFillRect(renderer, &other);
                }
                else{
                    other.x = broad_pack.players[i].x;
                    other.y = broad_pack.players[i].y;
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green player
                    SDL_RenderFillRect(renderer, &other);
                }
                
            }
        }
        
        // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green player
        // SDL_RenderFillRect(renderer, &player);
        SDL_RenderPresent(renderer);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    close(sock_fd);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
