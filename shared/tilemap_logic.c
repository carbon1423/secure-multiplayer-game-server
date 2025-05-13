#include "../client/tilemap.h"
#include "gameconfig.h"

Tile tilemap[MAP_HEIGHT][MAP_WIDTH];

void generate_test_map(void) {
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            if (row == MAP_HEIGHT - 1) {
                tilemap[row][col].type = TILE_SOLID;
            } else {
                tilemap[row][col].type = TILE_EMPTY;
            }
        }
    }
}
