#include "raylib.h"
#include "game.h"

int main(void) {

    InitWindow(GetScreenWidth(), GetScreenHeight(), "Farm Sim");
    SetTargetFPS(60);

    GameStartup();
    while(!WindowShouldClose() && gameState != GAME_STATE_EXIT)
    {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        
        if(gameState == GAME_STATE_MENU) {
            ClearBackground(RAYWHITE);
            DrawMainMenu();
            previousState = GAME_STATE_MENU;
        } else if (gameState == GAME_STATE_PLAYING) {
            GameRender();
            GameUpdate();
        } else if(gameState == GAME_STATE_PAUSED) {
            previousState = GAME_STATE_PAUSED;
            GameRender();
            DrawPauseMenu();
        } else if(gameState == GAME_STATE_OPTIONS) {
            ClearBackground(RAYWHITE);
            DrawOptionsMenu();
            if(IsKeyPressed(KEY_ESCAPE)) gameState = previousState;
        }

        EndDrawing();
    }

    GameShutDown();

    CloseWindow();  

    return 0;
}