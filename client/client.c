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
        WIN_LENGTH, WIN_HEIGHT, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

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
        int mouseX,mouseY;
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        Uint32 mouseState = SDL_GetMouseState(&mouseX,&mouseY);
        
        InputPacket input = {
            .type = 0,
            .left = keys[SDL_SCANCODE_A],
            .right = keys[SDL_SCANCODE_D],
            .mouseX = mouseX,
            .mouseY = mouseY
        };
        input.type = 0;
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_D]) {
            input.type |= INPUT_MOVE;
        }
        if (keys[SDL_SCANCODE_W]) {
            input.type |= INPUT_JUMP;
        }
        if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            input.type |= INPUT_SHOOT;
        }

        ssize_t sent = send(sock_fd, &input, sizeof(input), 0);
        if(sent < 0){
            perror("send");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
        SDL_RenderClear(renderer);
        BroadcastPacket broad_pack;
        int bytes_recv = recv(sock_fd, &broad_pack, sizeof(broad_pack), 0);
        SDL_Rect other;
        other.w = PLAYER_WIDTH;
        other.h = PLAYER_HEIGHT;
        SDL_Rect proj;
        proj.w = 10;
        proj.h = 5;

        if(bytes_recv > 0){
            for(int i = 0; i < broad_pack.count; i++){
                if(broad_pack.bullets[i].still_render){
                    proj.x = broad_pack.bullets[i].x;
                    proj.y = broad_pack.bullets[i].y;
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    SDL_RenderFillRect(renderer, &proj);
                }
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
