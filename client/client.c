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

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("SDL2 Client",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Rect player = {100, 100, 50, 50};
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
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w: player.y -= 10; break;
                    case SDLK_s: player.y += 10; break;
                    case SDLK_a: player.x -= 10; break;
                    case SDLK_d: player.x += 10; break;
                }
                struct MovementPacket packet;
                packet.type = 1;
                packet.x = player.x;
                packet.y = player.y;
                ssize_t sent = send(sock_fd, &packet, sizeof(packet), 0);
                if (sent < 0) {
                    perror("send");
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
        SDL_RenderClear(renderer);
        struct BroadcastPacket packet;
        int bytes_recv = recv(sock_fd, &packet, sizeof(packet), 0);
        SDL_Rect other;
        other.w = 50;
        other.h = 50;

        if(bytes_recv > 0){
            for(int i = 0; i < packet.count; i++){
                if(player_id != i){
                    other.x = packet.players[i].x;
                    other.y = packet.players[i].y;
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // blue player
                    SDL_RenderFillRect(renderer, &other);
                }
                
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green player
        SDL_RenderFillRect(renderer, &player);
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
