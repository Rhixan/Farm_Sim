#ifndef __WORLD_H__
#define __WORLD_H__

#define HOURS_PER_DAY 24
#define DAYS_PER_SEASON 7 
#define MINUTES_PER_HOUR 60
#define SECONDS_PER_MINUTE 60

#define TIME_SPEED 60.0f // 1 sec irl = 1 min in game

typedef enum {
    SEASON_SPRING,
    SEASON_SUMMER,
    SEASON_AUTUMN,
    SEASON_WINTER,
    SEASON_COUNT
} Season;

typedef struct {
    int year;
    int hour;
    int minute;
    int day;

    Season season;

    float timeSpeedMultiplier;
    float acumulatedseconds;

    float dayLightIntensity;
    Color NightColor;
    bool isNight;
} TimeSystem;

extern Vector2 clamped;

void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y);  

void InitWorld(void);

bool IsOutOfBounds(float x, float y);

bool CanMoveToPosition(float x, float y);

Vector2 ClampToWorld(float x, float y);

void InitTimeSystem(TimeSystem* ts);
void UpdateTimeSystem(TimeSystem* ts, float deltatime);
const char* GetSeasonName(Season season);

void UpdateDayLight(TimeSystem* ts);
float GetDayLight(const TimeSystem* ts);
bool isNightTime(const TimeSystem* ts);

#endif