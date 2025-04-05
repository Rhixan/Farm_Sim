#ifndef __WORLD_H__
#define __WORLD_H__

extern Vector2 clamped;

void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y);  

void InitWorld(void);

bool IsOutOfBounds(float x, float y);

bool CanMoveToPosition(float x, float y);

Vector2 ClampToWorld(float x, float y);

#endif