#include <stdio.h>            // printf, perror
#include <stdlib.h>           // exit, malloc
#include <string.h>           // memset
#include <unistd.h>           // close
#include <arpa/inet.h>        // sockaddr_in, inet_pton, htons
#include <sys/socket.h>       // socket, connect, send

struct MovementPacket {
    int type;
    float x;
    float y;
};

int main(){
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
    struct MovementPacket packet;
    packet.type = 1;
    packet.x = 100;
    packet.y = 100;

    while(1){
        packet.x += 10;
        packet.y += 10;
        ssize_t sent = send(sock_fd, &packet, sizeof(packet), 0);
        if (sent < 0) {
            perror("send");
        }
        sleep(1);
    }
}