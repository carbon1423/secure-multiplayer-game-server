#ifndef PACKET_H
#define PACKET_H


#define MAX_CLIENTS 16
#define MAX_PLAYERS 16

// Input type bit flags
#define INPUT_MOVE   0x1
#define INPUT_JUMP   0x2
#define INPUT_SHOOT  0x4

typedef struct {
    uint8_t type;    // bit flags (move, jump, shoot)
    uint8_t left;    // boolean
    uint8_t right;   // boolean
    int mouseX;
    int mouseY;
} InputPacket;

typedef struct {
    int active;
    int client_fd;
    float x, y;
    float vx, vy;
    int on_ground;
    int health;
} Player;

typedef struct {
    int still_render;
    float x, y;       // current position
    float vx, vy;     // velocity
} Bullet;

typedef struct {
    int client_fd;
    int player_id;
} ClientArgs;

typedef struct {
    int count;
    struct {
        int id;
        float x;
        float y;
        int remaining_health;
    } players[MAX_CLIENTS];
    
    struct {
        float x;
        float y;
        int still_render;
    } bullets[MAX_CLIENTS];
} BroadcastPacket;

#endif // PACKET_H
