#ifndef __GAME_H__
#define __GAME_H__

#include "world.h"
#include "items.h"
#include "constants.h"


extern const int screenWidth;
extern const int screenHeight;

extern Crop crops[MAX_CROPS];
extern int selectedCropType;
extern bool showPlowMarker;
extern Vector2 plowPosition;

extern ShopSystem shop;

extern InventorySystem inventory;

extern GameState gameState;
extern GameState previousState;

extern bool showEligibleTiles;

extern Camera2D camera;
extern sEntity player;

extern Texture2D textures[MAX_TEXTURES];

extern sTile world[WORLD_WIDTH][WORLD_WIDTH];

extern TimeSystem gameTime;

extern Sound sfxBuy;
extern Sound sfxSell;
extern Sound sfxClick;

void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y);

void DrawMainMenu();
void DrawPauseMenu(void);
void DrawOptionsMenu();

void GameStartup();
void GameUpdate();
void GameRender();
void GameShutDown();

void AddItemToInventory(ItemType type, int quantity);

#endif