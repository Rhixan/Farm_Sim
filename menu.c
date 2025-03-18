#include "raylib.h"
#include "menu.h"

void DrawMenu(int selectedOption) {
    ClearBackground(RAYWHITE);

    DrawText("Farm Sim", 250, 100, 50, DARKGREEN);
    DrawText("Start Game", 300, 200, 30, selectedOption == MENU_START_GAME ? RED : BLACK);
    DrawText("Options", 300, 250, 30, selectedOption == MENU_OPTIONS ? RED : BLACK);
    DrawText("Exit", 300, 300, 30, selectedOption == MENU_EXIT ? RED : BLACK);
}

MenuOption UpdateMenu(int selectedOption) {
    if (IsKeyPressed(KEY_DOWN)) {
        selectedOption++;
        if (selectedOption > MENU_EXIT) {
            selectedOption = MENU_START_GAME;
        }
    }
    if (IsKeyPressed(KEY_UP)) {
        selectedOption--;
        if (selectedOption < MENU_START_GAME) {
            selectedOption = MENU_EXIT;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        return selectedOption;
    }

    return MENU_NONE;
}