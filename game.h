#ifndef __GAME__
#define __GAME__

#define TILE_WIDTH 64
#define TILE_HEIGHT 64

#define WORLD_WIDTH 25
#define WORLD_HEIGHT 25

#define MAX_TEXTURES 1

#define GRASS_TILE_PROBABILITY 85 // 85%
#define TREE_TILE_PROBABILITY 12 // 12%
#define STONE_TILE_PROBABILITY 3 // 3%

extern const int screenWidth;
extern const int screenHeight;

typedef struct {
    int x;
    int y;
} sEntity;

typedef struct {
    int x;
    int y;
    int type;
} sTile;

typedef enum {
    TILE_TYPE_TREE = 0,
    TILE_TYPE_GRASS,
    TILE_TYPE_STONE,
    //TILE_TYPE_DIRT,
} tile_type;

typedef enum {
    TEXTURE_TILEMAP = 0
} texture_asset;

extern Camera2D camera;

extern sEntity player;

extern Texture2D textures[MAX_TEXTURES];

extern sTile world[WORLD_WIDTH][WORLD_WIDTH];

void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y);

void GameStartup();
void GameUpdate();
void GameRender();
void GameShutDown();

#endif