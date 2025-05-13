#ifndef TILEMAP_SHARED_H
#define TILEMAP_SHARED_H

#include "gameconfig.h"

typedef enum {
    TILE_EMPTY = 0,
    TILE_SOLID = 1
} TileType;

typedef struct {
    TileType type;
} Tile;

extern Tile tilemap[MAP_HEIGHT][MAP_WIDTH];

void generate_test_map(void);

int load_tilemap_from_file(const char* path);
#endif
