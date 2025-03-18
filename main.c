#include "raylib.h"
#include "menu.h"
#include <stdio.h>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Farm Sim");

    int selectedOption = MENU_START_GAME;
    bool inMenu = true;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (inMenu) {
            BeginDrawing();
            DrawMenu(selectedOption);
            EndDrawing();

            MenuOption result = UpdateMenu(selectedOption);
            if (result != MENU_NONE) {
                switch (result) {
                    case MENU_START_GAME:
                        inMenu = false;
                        break;
                    case MENU_OPTIONS:
                        break;
                    case MENU_EXIT:
                        CloseWindow();
                        return 0;
                    default:
                        break;
                }
            }
        } else {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Jocul a început!", 300, 250, 30, BLACK);
            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}