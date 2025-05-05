#ifndef PACKET_H
#define PACKET_H

#define MAX_CLIENTS 16
#define MAX_PLAYERS 16

struct BroadcastPacket {
    int count;
    struct {
        float x;
        float y;
    } players[MAX_CLIENTS];
};

struct MovementPacket {
    int type;
    float x;
    float y;
};

typedef struct {
    int active;
    int client_fd;
    float x, y;
} Player;

typedef struct {
    int client_fd;
    int player_id;
} ClientArgs;

#endif