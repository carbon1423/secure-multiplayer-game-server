#ifndef PACKET_H
#define PACKET_H

#define MAX_CLIENTS 16
#define MAX_PLAYERS 16
#define ACCEL 1.0f
#define FRICTION 0.7f
#define MAX_SPEED 10.0f  

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
    int type;
    int left;
    int right;
    int jump;
} InputPacket;

typedef struct {
    int active;
    int client_fd;
    float x, y;
    float vx, vy;
    int on_ground;
} Player;

typedef struct {
    int client_fd;
    int player_id;
} ClientArgs;

#endif