#ifndef __WORLD_H__
#define __WORLD_H__

#include "game.h"
#include "items.h"
#include "constants.h"

extern Vector2 clamped;

void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y);  

void InitWorld(void);

bool IsOutOfBounds(float x, float y);

bool CanMoveToPosition(float x, float y);

Vector2 ClampToWorld(float x, float y);

void InitTimeSystem(TimeSystem* ts);
void UpdateTimeSystem(TimeSystem* ts, float deltatime);
const char* GetSeasonName(Season season);

void UpdateDayLight(TimeSystem* ts, float deltaTime);

int GetBaseTextureX(int baseType);
int GetBaseTextureY(int baseType);

int GetObjectTextureX(int object_type);
int GetObjectTextureY(int object_type);

// Crops
void PlantCrop(int x, int y, ItemType seedType);
bool isCropAtPosition(int x, int y);
void HarvestCrop(int cropIndex);

void UpdateCropsByDay(const TimeSystem* ts);

void DrawShop();

bool IsBlockedByObject(int tileX, int tileY);

#endif