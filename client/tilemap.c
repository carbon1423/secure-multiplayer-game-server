#include <SDL2/SDL.h>
#include "tilemap.h"
#include "../shared/gameconfig.h"

void render_tilemap(SDL_Renderer* renderer){
    int x;
    int y;

    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
        if (tilemap[row][col].type == TILE_SOLID) {
            x = col * TILE_SIZE;
            y = row * TILE_SIZE;
            SDL_Rect tile = {x, y, TILE_SIZE, TILE_SIZE};
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderFillRect(renderer, &tile);
        }
        }
    }
}