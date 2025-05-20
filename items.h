#ifndef __ITEMS_H__
#define __ITEMS_H__

#include "raylib.h"
#include "game.h"
#include "world.h"
#include "constants.h"

typedef struct {
    float x;
    float y;
} sEntity;

typedef struct {
    int x;
    int y;
    int baseType;
    int objectType;
} sTile;

typedef enum {
    BASE_TYPE_GRASS = 0,
    BASE_TYPE_PLOWED,
    BASE_TYPE_COUNT
} base_type;

typedef enum {
    OBJECT_TYPE_NONE = 0,
    OBJECT_TYPE_TREE,
    OBJECT_TYPE_STONE,
    OBJECT_TYPE_SHOP,
    OBJECT_TYPE_COUNT
} object_type;

typedef enum {
    TEXTURE_TILEMAP = 0
} texture_asset;

// inventory

typedef enum {
    ITEM_NONE = 0,
    ITEM_HOE,
    ITEM_WATERING_CAN,
    ITEM_SEED_WHEET,
    ITEM_SEED_CORN,
    ITEM_SEED_POTATO,
    ITEM_SEED_BERRY,
    ITEM_WHEAT,
    ITEM_CORN,
    ITEM_POTATO,
    ITEM_BERRY,
    ITEM_COUTN
} ItemType;

typedef struct {
    ItemType type;
    int quantity;
    Texture2D texture;
} InventoryItem;

typedef struct {
    InventoryItem slots[INVENTORY_SLOTS];
    int selectedSlot;
} InventorySystem;

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

    float currentLight;
    float targetLight;
    float lightTransitionSpeed;

    float duskStart;
    float nightStart;
    float dawnStart;
    float dayStart;

    Color nightColor;
} TimeSystem;

// Crop system

typedef enum {
    CROP_NONE,
    CROP_WHEAT,
    CROP_CORN,
    CROP_POTATO,
    CROP_BERRY,
    CROP_COUNT
}CropType;

typedef struct {
    CropType type;
    int growthStage;
    float growthProgress;
    int waterLvl;
    bool isWatered;
    Vector2 position;
    Rectangle bounds;
}Crop;

// Shop system

typedef enum {
    SHOP_CATEGORY_NONE,
    SHOP_CATEGORY_SEEDS,
    SHOP_CATEGORY_PRODUCE,
    SHOP_CATEGORY_COUNT
} ShopCategory;

typedef struct {
    int playerMoney;
    
    // Item prices (changes daily)
    int seedPrices[CROP_COUNT];
    int producePrices[CROP_COUNT]; 
    
    // Shop location
    int x, y;
    bool isPlayerInShop;
    ShopCategory activeCategory;
} ShopSystem;

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_OPTIONS,
    GAME_STATE_EXIT
} GameState;

#endif