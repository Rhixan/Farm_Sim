#include "raylib.h"
#include "raymath.h"

#include "world.h"
#include "game.h"

sTile world[WORLD_WIDTH][WORLD_WIDTH];

void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y) {
    Rectangle source = {
        (float)texture_index_x * TILE_WIDTH + 0.5f,
        (float)texture_index_y * TILE_HEIGHT + 0.5f,
        (float)TILE_WIDTH - 1.0f,
        (float)TILE_HEIGHT - 1.0f 
    };
    Rectangle dest  = { (float)(pos_x), (float)(pos_y), (float)TILE_WIDTH, (float)TILE_HEIGHT};
    Vector2 origin = {0, 0};
    DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f, WHITE);
}

void InitWorld(void) {
    for (int i = 0; i < WORLD_WIDTH; i++) {
        for (int j = 0; j < WORLD_HEIGHT; j++) {
            int random = GetRandomValue(0, 100);
            world[i][j] = (sTile)
            {
                .x = i,
                .y = j,
            };
            if (random < GRASS_TILE_PROBABILITY) {
                world[i][j].type = TILE_TYPE_GRASS;
            } 
            else if (random < GRASS_TILE_PROBABILITY + TREE_TILE_PROBABILITY) {
                world[i][j].type = TILE_TYPE_TREE;
            }
            else {
                world[i][j].type = TILE_TYPE_STONE;
            }
            
        }
    }
}

bool IsOutOfBounds(float x, float y) {
    return ( x < 0 ||
        y < 0 ||
        x >= (WORLD_WIDTH * TILE_WIDTH) - TILE_WIDTH ||
        y >= (WORLD_HEIGHT * TILE_HEIGHT) - TILE_HEIGHT    
    );
}

Vector2 ClampToWorld(float x, float y) {
    return (Vector2) {
        Clamp(x, 0, (WORLD_WIDTH * TILE_WIDTH) - TILE_WIDTH),
        Clamp(y, 0, (WORLD_HEIGHT * TILE_HEIGHT) - TILE_HEIGHT)
    };
}