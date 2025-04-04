#include "raylib.h"
#include "raymath.h"
#include "game.h"

// #define TILE_WIDTH 64
// #define TILE_HEIGHT 64

// void GameStartup();
// void GameUpdate();
// void GameRender();
// void GameShutDown();
// void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y);

// const int screenWidth = 1000;
// const int screenHeight = 800;

// #define MAX_TEXTURES 1

// typedef enum {
//     TEXTURE_TILEMAP = 0
// } texture_asset;

// Texture2D textures[MAX_TEXTURES];

// #define WORLD_WIDTH 25
// #define WORLD_HEIGHT 25

// typedef enum {
//     TILE_TYPE_TREE = 0,
//     TILE_TYPE_GRASS,
//     TILE_TYPE_STONE,
//     //TILE_TYPE_DIRT,
// } tile_type;

// typedef struct {
//     int x;
//     int y;
//     int type;
// } sTile;

// sTile world[WORLD_WIDTH][WORLD_WIDTH];

// Camera2D camera = { 0 };

// typedef struct {
//     int x;
//     int y;
// } sEntity;

// sEntity player;

// void GameStartup() {

//     InitAudioDevice();

//     Image image = LoadImage("textures/mapPack_spritesheet.png");
//     textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
//     UnloadImage(image);

//     for (int i = 0; i < WORLD_WIDTH; i++) {
//         for (int j = 0; j < WORLD_HEIGHT; j++) {
//             world[i][j] = (sTile)
//             {
//                 .x = i,
//                 .y = j,
//                 .type = GetRandomValue(TILE_TYPE_TREE, TILE_TYPE_STONE)
//             };
//         }
//     }

//     player = (sEntity)
//     {
//         .x = TILE_WIDTH * 3,
//         .y = TILE_HEIGHT * 3
//     };

//     camera.target = (Vector2){player.x, player.y};
//     camera.offset = (Vector2){(float)screenWidth / 2, (float)screenHeight / 2};
//     camera.rotation = 0.0f;
//     camera.zoom = 2.0f;

// }
// void GameUpdate() {

//     float x = player.x;
//     float y = player.y;
//     if (IsKeyPressed(KEY_A)) {
//         x -= 0.5 * TILE_WIDTH;
//     } else if (IsKeyPressed(KEY_D)) {
//         x += 0.5 * TILE_WIDTH;
//     } else if (IsKeyPressed(KEY_W)) {
//         y -= 0.5 * TILE_HEIGHT;
//     } else if (IsKeyPressed(KEY_S)) {
//         y += 0.5 * TILE_HEIGHT;
//     }

//     float wheel = GetMouseWheelMove();
//     if (wheel != 0) {
//         const float zoomIncrement = 0.125f;
//         camera.zoom += (wheel * zoomIncrement);
//         if (camera.zoom < 0.7f) camera.zoom = 0.7f;
//         if (camera.zoom > 3.0f) camera.zoom = 3.0f;
//     }

//     player.x = x;
//     player.y = y;

//     camera.target = (Vector2){ player.x, player.y};

// }
// void GameRender() {

//     BeginMode2D(camera);

//     sTile tile;
//     int texture_index_x = 0;
//     int texture_index_y = 0;
//     for (int i = 0; i < WORLD_WIDTH; i++) {
//         for (int j = 0; j < WORLD_HEIGHT; j++) {
//             tile = world[i][j];
//             switch (tile.type) {
//                 case TILE_TYPE_TREE:
//                     texture_index_x = 13;
//                     texture_index_y = 6;
//                 break;

//                 case TILE_TYPE_GRASS:
//                     texture_index_x = 12;
//                     texture_index_y = 2;
//                 break;

//                 case TILE_TYPE_STONE:
//                     texture_index_x = 13;
//                     texture_index_y = 7;
//                 break;

//                 default:
//                     break;
//             }

//             DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, texture_index_x, texture_index_y);    
//         }
//     }

//     DrawTile(camera.target.x, camera.target.y, 1, 8);

//     EndMode2D();

//     //DrawRectangle(5, 5, 330, 120, Fade(SKYBLUE, 0.5f));
//     //DrawRectangleLines(5, 5, 330, 120, BLUE);

//     //DrawText(TextFormat("Camera Target: (%06.2f, %06.2f)", camera.target.x, camera.target.y), 15, 10, 14, YELLOW);
//     //DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 15, 30, 14, YELLOW);

// }
// void GameShutDown() {

//     for (int i = 0; i < MAX_TEXTURES; i++) {
//         UnloadTexture(textures[i]);
//     }

//     CloseAudioDevice();
// }

// void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y) {
//     Rectangle source = { (float)texture_index_x * TILE_WIDTH, (float)texture_index_y * TILE_HEIGHT, (float)TILE_WIDTH, (float)TILE_HEIGHT };
//     Rectangle dest  = { (float)(pos_x), (float)(pos_y), (float)TILE_WIDTH, (float)TILE_HEIGHT};
//     Vector2 origin = {0, 0};
//     DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f, WHITE);
// }


int main(void) {

    InitWindow(screenWidth, screenHeight, "Farm Sim");
    SetTargetFPS(60);

    GameStartup();
    while(!WindowShouldClose())
    {
        GameUpdate();

        BeginDrawing();
        ClearBackground(SKYBLUE);

        GameRender();

        EndDrawing();
    }

    GameShutDown();

    CloseWindow();  

    return 0;
}