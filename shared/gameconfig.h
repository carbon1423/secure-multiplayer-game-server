#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#define TILE_SIZE 32 // Width and height of each tile in pixels
#define MAP_WIDTH 25              // Number of tiles horizontally
#define MAP_HEIGHT 20             // Number of tiles vertically

#define WIN_HEIGHT (TILE_SIZE * MAP_HEIGHT)
#define WIN_LENGTH (TILE_SIZE * MAP_WIDTH)
#define PLAYER_HEIGHT 50
#define PLAYER_WIDTH 50

#define ACCEL 1.0f
#define FRICTION 0.7f
#define MAX_SPEED 10.0f  


#endif