#include "raylib.h"
#include "game.h"

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