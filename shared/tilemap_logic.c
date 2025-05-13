#include "../client/tilemap.h"
#include "gameconfig.h"
#include <stdio.h>

Tile tilemap[MAP_HEIGHT][MAP_WIDTH];

void generate_test_map(void) {
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            if (row == MAP_HEIGHT - 1) {
                tilemap[row][col].type = TILE_SOLID;
            }else if (row == MAP_HEIGHT - 4 && col > 3 && col < 6){
                tilemap[row][col].type = TILE_SOLID;
            }else {
                tilemap[row][col].type = TILE_EMPTY;
            }
        }
    }
}


int load_tilemap_from_file(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) return 0;

    char line[MAP_WIDTH + 2];  // +2 for newline + null terminator
    for (int row = 0; row < MAP_HEIGHT; row++) {
        if (!fgets(line, sizeof(line), file)) break;

        for (int col = 0; col < MAP_WIDTH; col++) {
            if (line[col] == '1') {
                tilemap[row][col].type = TILE_SOLID;
            } else {
                tilemap[row][col].type = TILE_EMPTY;
            }
        }
    }

    fclose(file);
    return 1;  // success
}
