#include <stdio.h>

#include "raylib.h"
#include "raymath.h"

#include "world.h"
#include "game.h"

sTile world[WORLD_WIDTH][WORLD_WIDTH];

void InitTimeSystem(TimeSystem* ts) {
    ts->hour = 19; // start at 6:00 AM
    ts->minute = 0;
    ts->day = 1;
    ts->year = 1;
    ts->season = SEASON_SPRING;
    ts->timeSpeedMultiplier = 1.0f;
    ts->acumulatedseconds = 0.0f;
}

void UpdateDayLight(TimeSystem* ts) {
    float hourProgress = (ts->hour + (ts->minute / 60.0f)) / HOURS_PER_DAY; 
    ts->dayLightIntensity = sinf(hourProgress * PI * 2.0f) * 0.5f + 0.5f;
    ts->dayLightIntensity = Clamp(ts->dayLightIntensity, 0.0f, 1.0f);

    ts->NightColor = (Color){10, 20, 50, 200};
    ts->isNight = (ts->hour < 6 || ts->hour >= 20);
}

float GetDayLight(const TimeSystem* ts) {
    return ts->dayLightIntensity;
}

bool isNightTime(const TimeSystem* ts) {
    return ts->isNight;
}

void UpdateTimeSystem(TimeSystem* ts, float deltatime) {
    float scaledDelta = deltatime * ts->timeSpeedMultiplier * TIME_SPEED;
    ts->acumulatedseconds += scaledDelta;

    while(ts->acumulatedseconds >= SECONDS_PER_MINUTE) {
        ts->acumulatedseconds -= SECONDS_PER_MINUTE;
        ts->minute++;

        // time progretion
        if(ts->minute >= MINUTES_PER_HOUR) {
            ts->minute -= MINUTES_PER_HOUR;
            ts->hour++;

            if(ts->hour >= HOURS_PER_DAY) {
                ts->hour = 0;
                ts->day++;

                if(ts->day >= DAYS_PER_SEASON) {
                    ts->day = 1;
                    ts->season = (ts->season + 1) % SEASON_COUNT;

                    if(ts->season == SEASON_SPRING) {
                        ts->year++;
                    }
                }
            }
            UpdateDayLight(ts);
        }
    }
}

const char* GetSeasonName(Season season) {
    static const char* seasonNames[] = {
        "Spring", "Summer", "Autumn", "Winter"
    };
    
    return seasonNames[season];
}


void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y) {
    Rectangle source = {
        (float)texture_index_x * TILE_WIDTH + 0.5f,
        (float)texture_index_y * TILE_HEIGHT + 0.5f,
        (float)TILE_WIDTH - 1.0f,
        (float)TILE_HEIGHT - 1.0f 
    };
    Rectangle dest  = { (float)(pos_x), (float)(pos_y), (float)TILE_WIDTH, (float)TILE_HEIGHT};
    Vector2 origin = {0, 0};
    DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f, WHITE);
}

void InitWorld(void) {
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
}

bool IsOutOfBounds(float x, float y) {
    return ( x < 0 ||
        y < 0 ||
        x >= (WORLD_WIDTH * TILE_WIDTH) - TILE_WIDTH ||
        y >= (WORLD_HEIGHT * TILE_HEIGHT) - TILE_HEIGHT    
    );
}

Vector2 ClampToWorld(float x, float y) {
    return (Vector2) {
        Clamp(x, 0, (WORLD_WIDTH * TILE_WIDTH) - TILE_WIDTH),
        Clamp(y, 0, (WORLD_HEIGHT * TILE_HEIGHT) - TILE_HEIGHT)
    };
}