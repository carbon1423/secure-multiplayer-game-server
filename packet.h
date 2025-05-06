#ifndef PACKET_H
#define PACKET_H


#define WIN_HEIGHT 300
#define WIN_LENGTH 600
#define PLAYER_HEIGHT 50
#define PLAYER_WIDTH 50
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
    struct {
        float x;
        float y;
    } bullets[MAX_CLIENTS];
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
    int mouseX,mouseY;
} InputPacket;

typedef struct {
    int active;
    int client_fd;
    float x, y;
    float vx, vy;
    int on_ground;
    int is_shooting; //0 no, 1 right, 2 left
} Player;
typedef struct {
    int still_render;
    float startX, startY;
    int x, y;
    int vx, vy;
} Bullet;

typedef struct {
    int client_fd;
    int player_id;
} ClientArgs;

#endif