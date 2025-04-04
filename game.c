#include "raylib.h"
#include "game.h"

const int screenWidth = 1000;
const int screenHeight = 800;

Camera2D camera = { 0 };
sEntity player;
Texture2D textures[MAX_TEXTURES];
sTile world[WORLD_WIDTH][WORLD_WIDTH];

void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y) {
    Rectangle source = { (float)texture_index_x * TILE_WIDTH, (float)texture_index_y * TILE_HEIGHT, (float)TILE_WIDTH, (float)TILE_HEIGHT };
    Rectangle dest  = { (float)(pos_x), (float)(pos_y), (float)TILE_WIDTH, (float)TILE_HEIGHT};
    Vector2 origin = {0, 0};
    DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f, WHITE);
}

void GameStartup() {

    InitAudioDevice();

    Image image = LoadImage("textures/mapPack_spritesheet.png");
    textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
    UnloadImage(image);

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

    player = (sEntity)
    {
        .x = TILE_WIDTH * 3,
        .y = TILE_HEIGHT * 3
    };

    camera.target = (Vector2){player.x + 20.0f, player.y + 20.0f };
    camera.offset = (Vector2){(float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;

}

void GameUpdate() {

    float x = player.x;
    float y = player.y;
    if (IsKeyDown(KEY_A)) {
        x -= 4;
    } else if (IsKeyDown(KEY_D)) {
        x += 4;
    } else if (IsKeyDown(KEY_W)) {
        y -= 4;
    } else if (IsKeyDown(KEY_S)) {
        y += 4;
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        const float zoomIncrement = 0.125f;
        camera.zoom += (wheel * zoomIncrement);
        if (camera.zoom < 0.7f) camera.zoom = 0.7f;
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    }

    player.x = x;
    player.y = y;

    camera.target = (Vector2){ player.x, player.y};

}
void GameRender() {

    BeginMode2D(camera);

    sTile tile;
    int texture_index_x = 0;
    int texture_index_y = 0;
    for (int i = 0; i < WORLD_WIDTH; i++) {
        for (int j = 0; j < WORLD_HEIGHT; j++) {
            tile = world[i][j];
            switch (tile.type) {
                case TILE_TYPE_TREE:
                    texture_index_x = 13;
                    texture_index_y = 6;
                break;

                case TILE_TYPE_GRASS:
                    texture_index_x = 12;
                    texture_index_y = 2;
                break;

                case TILE_TYPE_STONE:
                    texture_index_x = 13;
                    texture_index_y = 7;
                break;

                default:
                    break;
            }

            DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, texture_index_x, texture_index_y);    
        }
    }

    DrawTile(camera.target.x, camera.target.y, 1, 8);

    EndMode2D();

    //DrawRectangle(5, 5, 330, 120, Fade(SKYBLUE, 0.5f));
    //DrawRectangleLines(5, 5, 330, 120, BLUE);

    //DrawText(TextFormat("Camera Target: (%06.2f, %06.2f)", camera.target.x, camera.target.y), 15, 10, 14, YELLOW);
    //DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 15, 30, 14, YELLOW);

}

void GameShutDown() {

    for (int i = 0; i < MAX_TEXTURES; i++) {
        UnloadTexture(textures[i]);
    }

    CloseAudioDevice();
}