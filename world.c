#include <stdio.h>

#include "raylib.h"
#include "raymath.h"

#include "world.h"
#include "game.h"
#include "constants.h"

sTile world[WORLD_WIDTH][WORLD_WIDTH];

void InitTimeSystem(TimeSystem* ts) {
    ts->hour = 17; // start at 17:00 AM
    ts->minute = 0;
    ts->day = 1;
    ts->year = 1;
    ts->season = SEASON_SPRING;
    ts->timeSpeedMultiplier = 1.0f;
    ts->acumulatedseconds = 0.0f;

    ts->lightTransitionSpeed = 0.2f;
    ts->duskStart = 18.0f;
    ts->nightStart = 20.0f;
    ts->dawnStart = 4.0f;
    ts->dayStart = 6.0f;
    ts->nightColor = (Color){15, 25, 45, 255};
}

void UpdateDayLight(TimeSystem* ts, float deltaTime) {
    float currentHour = ts->hour + (ts->minute / 60.0f);

    if(currentHour >= ts->nightStart || currentHour < ts->dawnStart) {
        ts->targetLight = 0.0f; // full night
    }
    else if(currentHour >= ts->duskStart) {
        float progress = (currentHour - ts->duskStart) / (ts->nightStart - ts->duskStart);
        ts->targetLight = 1.0f - progress;
    }
    else if(currentHour >= ts->dawnStart) {
        float progress = (currentHour - ts->dawnStart) / (ts->dayStart - ts->dawnStart);
        ts->targetLight = progress;
    }
    else {
        ts->targetLight = 1.0f;
    }

    ts->currentLight = Lerp(ts->currentLight, ts->targetLight, deltaTime * ts->lightTransitionSpeed);
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
                .baseType = BASE_TYPE_GRASS,
                .objectType = OBJECT_TYPE_NONE
            };
            
            // Skip objects near the shop location
            if (i == SHOP_X && j == SHOP_Y) {
                continue;
            }
            
            if (random < TREE_TILE_PROBABILITY) {
                world[i][j].objectType = OBJECT_TYPE_TREE;
            } 
            else if(random < TREE_TILE_PROBABILITY + STONE_TILE_PROBABILITY) {
                world[i][j].objectType = OBJECT_TYPE_STONE;
            }
        }
    }
    
    // Clear space around the player's starting position
    for (int i = 2; i <= 4; i++) {
        for (int j = 2; j <= 4; j++) {
            world[i][j].objectType = OBJECT_TYPE_NONE;
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

int GetBaseTextureX(int baseType) {
    switch (baseType)
    {
    case BASE_TYPE_GRASS:
        return 13;
    case BASE_TYPE_PLOWED:
        return 9;
    default:
        return 0;
    }
}

int GetBaseTextureY(int baseType) {
    switch (baseType)
    {
    case BASE_TYPE_GRASS:
        return 0;
    case BASE_TYPE_PLOWED:
        return 1;
    default:
        return 0;
    }
}

int GetObjectTextureX(int objectType) {
    switch (objectType)
    {
    case OBJECT_TYPE_TREE:
        return 14;
    case OBJECT_TYPE_STONE:
        return 0;
    case OBJECT_TYPE_SHOP:
        return 0;
    default:
        return -1;
    }
}

int GetObjectTextureY(int objectType) {
    switch (objectType)
    {
    case OBJECT_TYPE_TREE:
        return 0;
    case OBJECT_TYPE_STONE:
        return 3;
    case OBJECT_TYPE_SHOP:
        return 17;
    default:
        return -1;
    }
}

bool isCropAtPosition(int x, int y) {
    for(int i = 0; i < MAX_CROPS; i++) {
        if(crops[i].type != CROP_NONE &&
            crops[i].position.x == x &&
            crops[i].position.y == y) {
                return true;
            }
    }
    return false;
}

CropType SeedToCropType(ItemType seedType) {
    switch (seedType) {
    case ITEM_SEED_WHEET: return CROP_WHEAT;
    case ITEM_SEED_POTATO: return CROP_POTATO;
    case ITEM_SEED_CORN: return CROP_CORN;
    case ITEM_SEED_BERRY: return CROP_BERRY;
    default: return CROP_NONE;
    }
}

void PlantCrop(int x, int y, ItemType seedType) {
    for(int i = 0; i < MAX_CROPS; i++) {
        if(crops[i].type == CROP_NONE) {
            crops[i] = (Crop){
                .type = SeedToCropType(seedType),
                .growthStage = 0,
                .growthProgress = 0.0f,
                .waterLvl = 0,
                .isWatered = false,
                .position = {x, y},
                .bounds = {
                    x * TILE_WIDTH, y * TILE_HEIGHT,
                    TILE_WIDTH, TILE_HEIGHT
                }
            };
            return;
        }
        TraceLog(LOG_INFO, "No Space left for crops");
    }
}

static int s_lastGlobalDay = -1;

// Converts (year,season,day) into days
static int GetGlobalDay(const TimeSystem* ts) {
    return (ts->year - 1) * (SEASON_COUNT * DAYS_PER_SEASON)
         + ts->season       * DAYS_PER_SEASON
         + (ts->day - 1);
}

void UpdateShopPrices() {
    for (int i = 1; i < CROP_COUNT; i++) {
        if (i == CROP_WHEAT)
            shop.producePrices[i] = GetRandomValue(2, 3);
        else if (i == CROP_CORN || i == CROP_POTATO)
            shop.producePrices[i] = GetRandomValue(3, 5);
        else if (i == CROP_BERRY)
            shop.producePrices[i] = GetRandomValue(1, 3);
            
        // Small chance for seed prices to change
        if (GetRandomValue(0, 100) < 20) {
            shop.seedPrices[i] = GetRandomValue(1, 2);
        }
    }
}

void UpdateCropsByDay(const TimeSystem* ts) {
    int today = GetGlobalDay(ts);
    if (today == s_lastGlobalDay) return;
    s_lastGlobalDay = today;

    // how much water drains per day
    float decay = WATER_LVL_MAX / (float)DAYS_TO_GROW;
    // days required per growth‐stage transition
    float daysPerStage = (float)DAYS_TO_GROW / (GROWTH_STAGES - 1);

    for (int i = 0; i < MAX_CROPS; i++) {
        if (crops[i].type == CROP_NONE) continue;

        // 1) Drain water
        crops[i].waterLvl -= decay;
        if (crops[i].waterLvl < 0) crops[i].waterLvl = 0;

        // 2) Grow only if still has water
        if (crops[i].waterLvl > 0) {
            // accumulate one in‐game day
            crops[i].growthProgress += 1.0f;
            // advance through as many stages as progress allows
            while (crops[i].growthStage < GROWTH_STAGES - 1
                   && crops[i].growthProgress >= daysPerStage) {
                crops[i].growthProgress -= daysPerStage;
                crops[i].growthStage++;
            }
            crops[i].isWatered = true;
        } else {
            // no water stops the growth
            crops[i].isWatered = false;
        }
    }
    UpdateShopPrices();
}

ItemType CropTypeToItemType(CropType cropType) {
    switch (cropType) {
        case CROP_WHEAT: return ITEM_WHEAT;
        case CROP_CORN: return ITEM_CORN;
        case CROP_POTATO: return ITEM_POTATO;
        case CROP_BERRY: return ITEM_BERRY;
        default: return ITEM_NONE;
    }
}

ItemType CropTypeToSeedType(CropType cropType) {
    switch (cropType) {
        case CROP_WHEAT: return ITEM_SEED_WHEET;
        case CROP_CORN: return ITEM_SEED_CORN;
        case CROP_POTATO: return ITEM_SEED_POTATO;
        case CROP_BERRY: return ITEM_SEED_BERRY;
        default: return ITEM_NONE;
    }
}

void HarvestCrop(int cropIndex) {
    if (cropIndex < 0 || cropIndex >= MAX_CROPS || crops[cropIndex].type == CROP_NONE) {
        return;
    }
    
    // Only fully grown crops can be harvested
    if (crops[cropIndex].growthStage < 3) {
        return;
    }

    CropType type = crops[cropIndex].type;
    ItemType seedType = CropTypeToSeedType(type);
    ItemType harvestType = CropTypeToItemType(type);
    
    // seed reward
    AddItemToInventory(seedType, 2);
    
    // harvest reward
    int quantity = (type == CROP_POTATO) ? GetRandomValue(2, 3) : 1;
    AddItemToInventory(harvestType, quantity);
    
    // Reset the field
    int x = crops[cropIndex].position.x;
    int y = crops[cropIndex].position.y;
    world[x][y].baseType = BASE_TYPE_GRASS;
    
    // Clear the crop
    crops[cropIndex].type = CROP_NONE;
    
    // Log the harvest
    const char* cropNames[] = {"None", "Wheat", "Potato", "Corn", "Berry"};
    TraceLog(LOG_INFO, "Harvested %s: +%d produce, +2 seeds", 
             cropNames[type], quantity);
}

void BuySeed(CropType cropType, int quantity) {
    if (cropType <= CROP_NONE || cropType >= CROP_COUNT) return;
    
    int totalCost = shop.seedPrices[cropType] * quantity;
    
    // Check if player has enough money
    if (shop.playerMoney >= totalCost) {
        ItemType seedType = CropTypeToSeedType(cropType);
        
        shop.playerMoney -= totalCost;
        PlaySound(sfxBuy);
        AddItemToInventory(seedType, quantity);
    }
}

void SellItem(ItemType itemType, int quantity) {
    int price = 0;
    CropType cropType = CROP_NONE;
    
    // Determine crop type and price based on item
    if (itemType == ITEM_WHEAT) cropType = CROP_WHEAT;
    else if (itemType == ITEM_CORN) cropType = CROP_CORN;
    else if (itemType == ITEM_POTATO) cropType = CROP_POTATO;
    else if (itemType == ITEM_BERRY) cropType = CROP_BERRY;
    else if (itemType == ITEM_SEED_WHEET) cropType = CROP_WHEAT;
    else if (itemType == ITEM_SEED_CORN) cropType = CROP_CORN;
    else if (itemType == ITEM_SEED_POTATO) cropType = CROP_POTATO;
    else if (itemType == ITEM_SEED_BERRY) cropType = CROP_BERRY;
    
    if (cropType != CROP_NONE) {
        if (itemType == ITEM_SEED_WHEET || itemType == ITEM_SEED_CORN || itemType == ITEM_SEED_POTATO || itemType == ITEM_SEED_BERRY) {
            price = shop.seedPrices[cropType];
        } else {
            price = shop.producePrices[cropType];
        }
        
        // Remove from inventory and add money
        for (int i = 0; i < INVENTORY_SLOTS; i++) {
            if (inventory.slots[i].type == itemType) {
                int toSell = quantity > inventory.slots[i].quantity ? inventory.slots[i].quantity : quantity;
                inventory.slots[i].quantity -= toSell;
                shop.playerMoney += toSell * price;
                PlaySound(sfxSell);
                
                if (inventory.slots[i].quantity <= 0) {
                    inventory.slots[i].type = ITEM_NONE;
                    if (inventory.slots[i].texture.id != 0) {
                        UnloadTexture(inventory.slots[i].texture);
                    }
                    inventory.slots[i].texture = (Texture2D){0};
                }
                break;
            }
        }
    }
}

// Helper to get inventory count for an item
static int GetInventoryCount(ItemType type) {
    for (int i = 0; i < INVENTORY_SLOTS; i++) {
        if (inventory.slots[i].type == type) {
            return inventory.slots[i].quantity;
        }
    }
    return 0;
}

// Helper to draw a shop row (buy/sell)
static void DrawShopRow(const char* label, int price, int have, Rectangle btnRect, const char* btnText, Color btnColor, Color textColor, bool showBtn) {
    int fontSize = 15;
    int labelWidth = MeasureText(label, fontSize);
    char priceStr[64];
    snprintf(priceStr, sizeof(priceStr), "%d gold (Have: %d)", price, have);
    int priceWidth = MeasureText(priceStr, fontSize);
    int padding = 10;
    int labelX = btnRect.x - (labelWidth + priceWidth + 2 * padding + 10);
    int labelY = btnRect.y + 6;
    int priceX = btnRect.x - (priceWidth + padding);
    int priceY = btnRect.y + 6;
    DrawText(label, labelX, labelY, fontSize, BLACK);
    DrawText(priceStr, priceX, priceY, fontSize, DARKGRAY);
    if (showBtn) {
        DrawRectangleRec(btnRect, btnColor);
        int btnTextWidth = MeasureText(btnText, fontSize);
        DrawText(btnText, btnRect.x + (btnRect.width - btnTextWidth) / 2, btnRect.y + 5, fontSize, textColor);
    }
}

void DrawShop(void) {
    if (shop.activeCategory == SHOP_CATEGORY_NONE) {
        if (shop.isPlayerInShop) {
            Vector2 screenPos = GetWorldToScreen2D((Vector2){ shop.x * TILE_WIDTH, shop.y * TILE_HEIGHT - 20 }, camera);
            DrawText("Press SPACE to open shop", screenPos.x, screenPos.y, 10, WHITE);
        }
        return;
    }

    int shopWidth  = 340;
    int shopHeight = 500;
    int shopX = GetScreenWidth()/2 - shopWidth/2;
    int shopY = GetScreenHeight()/2 - shopHeight/2;
    DrawRectangle(shopX, shopY, shopWidth, shopHeight, Fade(BEIGE, 0.9f));
    DrawRectangleLines(shopX, shopY, shopWidth, shopHeight, BROWN);

    DrawText("FARM SHOP", shopX + shopWidth/2 - MeasureText("FARM SHOP", 20)/2, shopY + 10, 20, DARKBROWN);
    DrawText(TextFormat("Money: %d gold", shop.playerMoney), shopX + 10, shopY + 40, 18, YELLOW);

    int tabWidth = shopWidth / (SHOP_CATEGORY_COUNT - 1);
    for (int i = 1; i < SHOP_CATEGORY_COUNT; i++) {
        Rectangle tabRect = { shopX + (i-1)*tabWidth, shopY + 70, tabWidth, 30 };
        Color tabColor = (i == shop.activeCategory) ? GOLD : LIGHTGRAY;
        DrawRectangleRec(tabRect, tabColor);
        DrawRectangleLinesEx(tabRect, 1, DARKBROWN);
        const char* tabName = (i == SHOP_CATEGORY_SEEDS) ? "SEEDS" : "PRODUCE";
        int tabTextWidth = MeasureText(tabName, 16);
        DrawText(tabName, tabRect.x + (tabRect.width - tabTextWidth)/2, tabRect.y + 8, 16, DARKBROWN);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), tabRect)) {
            shop.activeCategory = i;
            PlaySound(sfxClick);
        }
    }

    int itemY = shopY + 110;
    if (shop.activeCategory == SHOP_CATEGORY_SEEDS) {
        DrawText("BUY SEEDS", shopX + 10, itemY, 18, DARKBROWN);
        itemY += 32;
        const char* seedNames[] = {"", "Wheat", "Corn", "Potato", "Berries"};
        for (int i = 1; i < CROP_COUNT; i++) {
            Rectangle buyRect = { shopX + shopWidth - 80, itemY - 5, 60, 25 };
            int have = GetInventoryCount(CropTypeToSeedType(i));
            DrawShopRow(TextFormat("%s Seed", seedNames[i]), shop.seedPrices[i], have, buyRect, "BUY", SKYBLUE, DARKBLUE, 1);
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), buyRect)) {
                BuySeed(i, 1);
            }
            itemY += 35;
        }
    } else {
        DrawText("SELL ITEMS", shopX + 10, itemY, 18, DARKBROWN);
        itemY += 32;
        const char* itemTypes[] = {"Seeds", "Produce"};
        const char* cropNames[] = {"", "Wheat", "Corn", "Potato", "Berries"};
        for (int type = 0; type < 2; type++) {
            DrawText(itemTypes[type], shopX + 20, itemY, 16, DARKBROWN);
            itemY += 25;
            for (int i = 1; i < CROP_COUNT; i++) {
                int price = (type == 0) ? shop.seedPrices[i] : shop.producePrices[i];
                ItemType itemToSell = (type == 0) ? CropTypeToSeedType(i) : CropTypeToItemType(i);
                int have = GetInventoryCount(itemToSell);
                Rectangle sellRect = { shopX + shopWidth - 80, itemY - 5, 60, 25 };
                DrawShopRow(TextFormat("%s%s", cropNames[i], (type == 0) ? " Seed" : ""), price, have, sellRect, "SELL", GREEN, DARKGREEN, have > 0);
                if (have > 0 && IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), sellRect)) {
                    SellItem(itemToSell, 1);
                }
                itemY += 30;
            }
            itemY += 12;
        }
    }
    DrawText("Press ESC to close the Farm Shop", shopX + shopWidth/2 - MeasureText("Press ESC to close the Farm Shop", 16)/2, shopY + shopHeight - 30, 16, GRAY);
}

bool IsBlockedByObject(int tileX, int tileY) {
    if (tileX < 0 || tileX >= WORLD_WIDTH || tileY < 0 || tileY >= WORLD_HEIGHT)
        return true; // treat out of bounds as blocked
    int obj = world[tileX][tileY].objectType;
    // printf("Check (%d,%d): obj=%d\n", tileX, tileY, obj); 
    return (obj == OBJECT_TYPE_TREE || obj == OBJECT_TYPE_STONE || obj == OBJECT_TYPE_SHOP);
}



