#include <stdio.h>

#include "raylib.h"
#include "raymath.h"

#include "game.h"
#include "world.h"

const int screenWidth = 1000;
const int screenHeight = 800;

Camera2D camera = { 0 };
sEntity player;
Texture2D textures[MAX_TEXTURES];
TimeSystem gameTime;

void GameStartup() {

    InitAudioDevice();

    InitTimeSystem(&gameTime);

    Image image = LoadImage("textures/mapPack_spritesheet.png");
    textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
    UnloadImage(image);

    InitWorld();

    player = (sEntity)
    {
        .x = TILE_WIDTH * 3,
        .y = TILE_HEIGHT * 3
    };

    camera.target = Vector2Lerp (camera.target, (Vector2){player.x, player.y}, 0.1f);
    camera.offset = (Vector2){(float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;

}

void GameUpdate() {

    float x = player.x;
    float y = player.y;

    float moveSpeed = 4.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        x -= moveSpeed;
    } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        x += moveSpeed;
    } else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        y -= moveSpeed;
    } else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN   )) {
        y += moveSpeed;
    }

    if(!IsOutOfBounds(x, y)) {
        player.x = x;
        player.y = y;
    } else {
        Vector2 clamped = ClampToWorld(x, y);
        player.x = clamped.x;
        player.y = clamped.y;
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        const float zoomIncrement = 0.125f;
        camera.zoom += (wheel * zoomIncrement);
        if (camera.zoom < 0.7f) camera.zoom = 0.7f;
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    }

    camera.target = (Vector2){ player.x, player.y};

    UpdateTimeSystem(&gameTime, GetFrameTime());

    // Debug controls for time speed
    if (IsKeyPressed(KEY_Q)) gameTime.timeSpeedMultiplier *= 2.0f;
    if (IsKeyPressed(KEY_E)) gameTime.timeSpeedMultiplier *= 0.5f;
    if (IsKeyPressed(KEY_R)) gameTime.timeSpeedMultiplier = 1.0f;

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

    if(gameTime.isNight) {
        float darkness = 1.0f - gameTime.dayLightIntensity;
        DrawRectangle(0, 0, screenWidth, screenHeight,
            ColorAlpha(gameTime.NightColor, darkness * 0.7f)
        );
        
        DrawCircleGradient(
            (player.x - camera.target.x) + screenWidth/2,
            (player.y - camera.target.y) + screenHeight/2,
            150,
            ColorAlpha(YELLOW, 0.2f),
            BLANK
        );
    }

    char timeText[50];
    snprintf(timeText, sizeof(timeText), "Year %d, Day %d \n%s  %02d:%02d x%.1f",
        gameTime.year,
        gameTime.day,
        GetSeasonName(gameTime.season),
        gameTime.hour,
        gameTime.minute,
        gameTime.timeSpeedMultiplier
    );

    DrawText(timeText, 10, 10, 20, BLACK);

    //char speedText[50];
    // snprintf(speedText, sizeof(speedText), "Time Speed: %.1fx", gameTime.timeSpeedMultiplier);
    // DrawText(speedText, 10, 40, 20, RED);

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