#ifndef __MENU_H__
#define __MENU_H__

#include "raylib.h"

typedef enum {
    MENU_START_GAME,
    MENU_OPTIONS,
    MENU_EXIT,
    MENU_NONE 
} MenuOption;


void DrawMenu(int selectedOption); 
MenuOption UpdateMenu(int selectedOption); 

#endif