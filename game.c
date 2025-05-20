#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"

#include "game.h"
#include "world.h"

static const int resolutions[][2] = {
    {800, 600},
    {1024, 768},
    {1280, 720},
    {1280, 800},
    {1600, 900},
    {1920, 1080}
};
static int currentResolution = 3; // Default index (1280x800)

float sfxVolume = 0.5f;
float musicVolume = 0.1f;

Camera2D camera = { 0 };
sEntity player;
Texture2D textures[MAX_TEXTURES];
TimeSystem gameTime;

ShopSystem shop = {0};

Crop crops[MAX_CROPS] = {0};
int selectedCropType = CROP_WHEAT;

Sound sfxWalk;
Sound sfxPlow;
Sound sfxPlant;
Sound sfxWater;
Sound sfxHarvest;
Sound sfxBuy;
Sound sfxSell;
Sound sfxClick;

Music bgMusic;

bool showEligibleTiles = false;

bool IsTileEligibleForPlowing(int tileX, int tileY) {
    if(tileX < 0 || tileX >= WORLD_WIDTH || tileY < 0 || tileY >= WORLD_HEIGHT) {
        return false;
    }

    sTile tile = world[tileX][tileY];

    return (tile.objectType == OBJECT_TYPE_NONE &&
            (tile.baseType == BASE_TYPE_GRASS));
}

// Inventory
InventorySystem inventory = {0};

GameState gameState = GAME_STATE_MENU;
GameState previousState = GAME_STATE_MENU;

// Maps item types to sprite coordinates in the tilemap
typedef struct {
    int x, y;
} SpriteCoord;

static const SpriteCoord itemSpriteCoords[] = {
    {0, 0},                // ITEM_NONE
    {3, 16},               // ITEM_HOE
    {4, 16},               // ITEM_WATERING_CAN
    {5, 11},               // ITEM_SEED_WHEET
    {5, 15},               // ITEM_SEED_CORN 
    {5, 13},               // ITEM_SEED_POTATO
    {11, 13},              // ITEM_SEED_BERRY
    {0, 11},               // ITEM_WHEAT
    {0, 15},               // ITEM_CORN
    {0, 13},               // ITEM_POTATO
    {6, 13}                // ITEM_BERRY
};

// Creates an icon texture from the sprite at the given coordinates
static Texture2D CreateIconFromSpritesheet(Texture2D sheet, int spriteX, int spriteY) {
    Image img = ImageFromImage(
        LoadImageFromTexture(sheet),
        (Rectangle){ 
            spriteX * TILE_WIDTH, 
            spriteY * TILE_HEIGHT, 
            TILE_WIDTH, 
            TILE_HEIGHT 
        }
    );
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

void DrawMainMenu(void) {
    const char* title = "Farm Sim";
    int titleWidth = MeasureText(title, 48);
    DrawText(title, GetScreenWidth()/2 - titleWidth/2, GetScreenHeight()/2 - titleWidth, 48, DARKGREEN);

    const char* options[] = { "START GAME", "OPTIONS", "EXIT" };
    int optionCount = 3;
    int selected = -1;

    int btnWidth = 260, btnHeight = 50, spacing = 24;
    int startY = GetScreenHeight() / 2 - btnHeight * 2;

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < optionCount; i++) {
        int btnX = GetScreenWidth()/2 - btnWidth/2;
        int btnY = startY + i * (btnHeight + spacing);
        Rectangle btnRect = { btnX, btnY, btnWidth, btnHeight };

        Color btnColor = CheckCollisionPointRec(mouse, btnRect) ? SKYBLUE : LIGHTGRAY;
        DrawRectangleRec(btnRect, btnColor);
        DrawRectangleLinesEx(btnRect, 2, DARKGRAY);

        int textWidth = MeasureText(options[i], 28);
        DrawText(options[i], btnX + btnWidth/2 - textWidth/2, btnY + 10, 28, DARKGREEN);

        if (CheckCollisionPointRec(mouse, btnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selected = i;
        }
    }

    // Handle selection
    if (selected == 0) gameState = GAME_STATE_PLAYING;
    else if (selected == 1) gameState = GAME_STATE_OPTIONS;
    else if (selected == 2) gameState = GAME_STATE_EXIT;
}

void DrawPauseMenu(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, 0.7f));

    const char* options[] = { "CONTINUE GAME", "OPTIONS", "EXIT TO MAIN MENU" };
    int optionCount = 3;
    int selected = -1;
    int btnWidth = 300, btnHeight = 50, spacing = 24;
    int startY = GetScreenHeight() / 2 - btnHeight * 2;

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < optionCount; i++) {
        int btnX = GetScreenWidth()/2 - btnWidth/2;
        int btnY = startY + i * (btnHeight + spacing);
        Rectangle btnRect = { btnX, btnY, btnWidth, btnHeight };

        Color btnColor = CheckCollisionPointRec(mouse, btnRect) ? SKYBLUE : LIGHTGRAY;
        DrawRectangleRec(btnRect, btnColor);
        DrawRectangleLinesEx(btnRect, 2, DARKGRAY);

        int textWidth = MeasureText(options[i], 28);
        DrawText(options[i], btnX + btnWidth/2 - textWidth/2, btnY + 10, 28, DARKGREEN);

        if (CheckCollisionPointRec(mouse, btnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selected = i;
        }
    }

    // Handle selection
    if (selected == 0) gameState = GAME_STATE_PLAYING;
    else if (selected == 1) gameState = GAME_STATE_OPTIONS;
    else if (selected == 2) gameState = GAME_STATE_MENU;
}

void DrawOptionsMenu(void) {
    int x = GetScreenWidth()/2 - 200;
    int w = 400, h = 300;
    int y = GetScreenHeight()/2 - h/2;

    DrawRectangle(x, y, w, h, Fade(LIGHTGRAY, 0.95f));
    DrawRectangleLines(x, y, w, h, DARKGRAY);

    DrawText("OPTIONS", x + 120, y + 20, 32, DARKGREEN);

    // --- Resolution ---
    DrawText("Resolution:", x + 30, y + 80, 22, BLACK);
    char resText[32];
    sprintf(resText, "%dx%d", resolutions[currentResolution][0], resolutions[currentResolution][1]);

    Rectangle leftBtn = {x + 170, y + 80, 24, 24};
    Rectangle rightBtn = {x + 330, y + 80, 24, 24}; // move right button further right
    DrawRectangleRec(leftBtn, LIGHTGRAY);
    DrawRectangleRec(rightBtn, LIGHTGRAY);
    DrawText("<", leftBtn.x + 5, leftBtn.y, 22, DARKGRAY);
    DrawText(">", rightBtn.x + 5, rightBtn.y, 22, DARKGRAY);

    // Center the text between the buttons
    int textWidth = MeasureText(resText, 22);
    int textX = leftBtn.x + leftBtn.width + ((rightBtn.x - (leftBtn.x + leftBtn.width)) - textWidth) / 2;
    DrawText(resText, textX, y + 80, 22, DARKBLUE);

    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, leftBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (currentResolution > 0) {
            currentResolution--;
            // Check for fullscreen resolution
            if (resolutions[currentResolution][0] == 1920 && resolutions[currentResolution][1] == 1080) {
                SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                if (!IsWindowFullscreen()) ToggleFullscreen();
            } else {
                if (IsWindowFullscreen()) ToggleFullscreen();
                SetWindowSize(resolutions[currentResolution][0], resolutions[currentResolution][1]);
        }
    }
    }
    if (CheckCollisionPointRec(mouse, rightBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (currentResolution < (sizeof(resolutions)/sizeof(resolutions[0]))-1) {
            currentResolution++;
            // Check for fullscreen resolution
            if (resolutions[currentResolution][0] == 1920 && resolutions[currentResolution][1] == 1080) {
                SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                if (!IsWindowFullscreen()) ToggleFullscreen();
            } else {
                if (IsWindowFullscreen()) ToggleFullscreen();
                SetWindowSize(resolutions[currentResolution][0], resolutions[currentResolution][1]);
            }
        }
    }

    // --- SFX Volume ---
    DrawText("SFX Volume:", x + 30, y + 130, 22, BLACK);
    Rectangle sfxBar = {x + 200, y + 135, 150, 12};
    DrawRectangleRec(sfxBar, DARKGRAY);
    DrawRectangle(x + 200, y + 135, (int)(sfxVolume * 150), 12, GREEN);
    if (CheckCollisionPointRec(mouse, sfxBar) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        sfxVolume = (mouse.x - sfxBar.x) / sfxBar.width;
        if (sfxVolume < 0) sfxVolume = 0;
        if (sfxVolume > 1) sfxVolume = 1;
        SetSoundVolume(sfxWalk, sfxVolume);
        SetSoundVolume(sfxPlow, sfxVolume);
        SetSoundVolume(sfxPlant, sfxVolume);
        SetSoundVolume(sfxWater, sfxVolume);
        SetSoundVolume(sfxHarvest, sfxVolume);
        SetSoundVolume(sfxBuy, sfxVolume);
        SetSoundVolume(sfxSell, sfxVolume);
        SetSoundVolume(sfxClick, sfxVolume);
    }

    // --- Music Volume ---
    DrawText("Music Volume:", x + 30, y + 180, 22, BLACK);
    Rectangle musicBar = {x + 200, y + 185, 150, 12};
    DrawRectangleRec(musicBar, DARKGRAY);
    DrawRectangle(x + 200, y + 185, (int)(musicVolume * 150), 12, BLUE);
    if (CheckCollisionPointRec(mouse, musicBar) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        musicVolume = (mouse.x - musicBar.x) / musicBar.width;
        if (musicVolume < 0) musicVolume = 0;
        if (musicVolume > 1) musicVolume = 1;
        SetMusicVolume(bgMusic, musicVolume);
    }

    // --- Back Button ---
    Rectangle backBtn = {x + 120, y + 240, 160, 40};
    DrawRectangleRec(backBtn, LIGHTGRAY);
    DrawRectangleLinesEx(backBtn, 2, DARKGRAY);
    DrawText("BACK", x + 170, y + 250, 24, DARKGREEN);
    if (CheckCollisionPointRec(mouse, backBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        gameState = previousState;
    }
}

void InitInventory() {
    // Reset inventory slots
    for(int i = 0; i < INVENTORY_SLOTS; i++) {
        inventory.slots[i].type = ITEM_NONE;
        inventory.slots[i].quantity = 0;
        inventory.slots[i].texture = (Texture2D){0};
    }
    
    // Standard starting items
    inventory.slots[0].type = ITEM_HOE;
    inventory.slots[0].quantity = 1;
    
    inventory.slots[1].type = ITEM_WATERING_CAN;
    inventory.slots[1].quantity = 1;
    
    inventory.slots[2].type = ITEM_SEED_WHEET;
    inventory.slots[2].quantity = 5;

    inventory.slots[3].type = ITEM_SEED_CORN;
    inventory.slots[3].quantity = 5;
    
    // Set icons for non-empty slots
    for(int i = 0; i < INVENTORY_SLOTS; i++) {
        ItemType type = inventory.slots[i].type;
        if(type != ITEM_NONE) {
            SpriteCoord coords = itemSpriteCoords[type];
            inventory.slots[i].texture = CreateIconFromSpritesheet(
                textures[TEXTURE_TILEMAP], 
                coords.x, 
                coords.y
            );
        }
    }
    
    inventory.selectedSlot = 0;
}

void UpdateInventory() {
    // changes slot by pressing 1-0 bt
    for(int i = 0; i < 10; i++) {
        if(IsKeyPressed(KEY_ONE + i)) {
            inventory.selectedSlot = i % INVENTORY_SLOTS;
            if(IsKeyPressed(KEY_ZERO)) inventory.selectedSlot = 9;
        }
    }

    // using item with E key
    if(IsKeyPressed(KEY_E)) {
        ItemType selectedItem = inventory.slots[inventory.selectedSlot].type;
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        int TileX = (int)(mouseWorldPos.x / TILE_WIDTH);
        int TileY = (int)(mouseWorldPos.y / TILE_HEIGHT);

        // plowing
        if(selectedItem == ITEM_HOE) {
            if(IsTileEligibleForPlowing(TileX, TileY)) {
                PlaySound(sfxPlow);
                world[TileX][TileY].baseType = BASE_TYPE_PLOWED;
            }
        }

        // planting
        if(selectedItem >= ITEM_SEED_WHEET && selectedItem <= ITEM_SEED_BERRY) {
            if(world[TileX][TileY].baseType == BASE_TYPE_PLOWED && !isCropAtPosition(TileX, TileY)) {
                PlantCrop(TileX, TileY, selectedItem);
                PlaySound(sfxPlant);
                // removes seed from inventory
                inventory.slots[inventory.selectedSlot].quantity--;
                if(inventory.slots[inventory.selectedSlot].quantity <= 0) {
                    inventory.slots[inventory.selectedSlot].type = ITEM_NONE;
                }
            }
        }

        // watering
        if (selectedItem == ITEM_WATERING_CAN) {
            for (int c = 0; c < MAX_CROPS; c++) {
                if (crops[c].type != CROP_NONE &&
                    crops[c].position.x == TileX &&
                    crops[c].position.y == TileY) {
                    crops[c].waterLvl   = WATER_LVL_MAX;
                    crops[c].isWatered  = true;
                    PlaySound(sfxWater);
                    break;
                }
            }
        }
    }

    // Harvesting with right click
    if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        int TileX = (int)(mouseWorldPos.x / TILE_WIDTH);
        int TileY = (int)(mouseWorldPos.y / TILE_HEIGHT);
        
        // Check if there's a mature crop at this position
        for(int i = 0; i < MAX_CROPS; i++) {
            if(crops[i].type != CROP_NONE && 
               crops[i].position.x == TileX && 
               crops[i].position.y == TileY &&
               crops[i].growthStage == 3) {
                HarvestCrop(i);
                PlaySound(sfxHarvest);
                break;
            }
        }
    }
}

void AddItemToInventory(ItemType type, int quantity) {
    if(type == ITEM_NONE || quantity <= 0) return;
    
    // First check if we already have this item
    for(int i = 0; i < INVENTORY_SLOTS; i++) {
        if(inventory.slots[i].type == type) {
            inventory.slots[i].quantity += quantity;
            return;
        }
    }
    
    // Find first empty slot
    for(int i = 0; i < INVENTORY_SLOTS; i++) {
        if(inventory.slots[i].type == ITEM_NONE) {
            inventory.slots[i].type = type;
            inventory.slots[i].quantity = quantity;
            inventory.slots[i].texture = CreateIconFromSpritesheet(
                textures[TEXTURE_TILEMAP],
                itemSpriteCoords[type].x,
                itemSpriteCoords[type].y
            );
            return;
        }
    }
    
    TraceLog(LOG_WARNING, "Inventory full - couldn't add item %d", type);
}

// Update GetItemName to include harvested crops
static const char* GetItemName(ItemType type) {
    switch(type) {
        case ITEM_HOE:           return "Hoe";
        case ITEM_WATERING_CAN:  return "Watering Can";
        case ITEM_SEED_WHEET:    return "Wheat Seed";
        case ITEM_SEED_CORN:     return "Corn Seed";
        case ITEM_SEED_POTATO:   return "Potato Seed";
        case ITEM_SEED_BERRY:    return "Berry Seed";
        case ITEM_WHEAT:         return "Wheat";
        case ITEM_CORN:          return "Corn";
        case ITEM_POTATO:        return "Potato";
        case ITEM_BERRY:         return "Beries";
        default:                 return "";
    }
}

void DrawInventory(void) {
    const int slotSize    = 40;
    const int slotMargin  = 3;
    const int barHeight   = slotSize + slotMargin;
    const int totalWidth  = INVENTORY_SLOTS*slotSize + (INVENTORY_SLOTS-1)*slotMargin;
    const int barX        = GetScreenWidth()/2 - totalWidth/2;
    const int barY        = GetScreenHeight() - barHeight - 10;

    // Draw semi‐transparent panel
    DrawRectangle(barX - 5, barY - 5, totalWidth + 10, barHeight + 10, Fade(BLACK, 0.5f));

    // Draw each slot
    for (int i = 0; i < INVENTORY_SLOTS; i++) {
        int x = barX + i*(slotSize + slotMargin);
        int y = barY + slotMargin;
        Rectangle r = { x, y, slotSize, slotSize };

        // slot background
        Color bg = (i == inventory.selectedSlot) ? Fade(GOLD, 0.8f) : Fade(LIGHTGRAY, 0.7f);
        DrawRectangleRounded(r, 0.15f, 4, bg);
        DrawRectangleRoundedLines(r, 0.15f, 4, DARKGRAY);

        // draw icon or placeholder
        if (inventory.slots[i].type != ITEM_NONE) {
            Texture2D icon = inventory.slots[i].texture; // assign this in InitInventory
            if (icon.id != 0) {
                DrawTexturePro(icon,
                    (Rectangle){ 0,0, icon.width, icon.height },
                    (Rectangle){ x, y, slotSize, slotSize },
                    (Vector2){0,0}, 0, WHITE);
            } else {
                const char *name = TextFormat("%d", inventory.slots[i].type);
                DrawText(name, x + slotSize/4, y + slotSize/4, 14, BLACK);
            }
            // quantity badge
            if (inventory.slots[i].quantity > 1) {
                int badgeX = x + slotSize - 18;
                int badgeY = y + slotSize - 18;
                int badgeW = 16, badgeH = 16;
                // Draw background for quantity
                DrawRectangleRounded((Rectangle){badgeX, badgeY, badgeW, badgeH}, 0.3f, 6, Fade(BLACK, 0.7f));
                // Draw quantity text centered in the badge
                const char* qtyText = TextFormat("%d", inventory.slots[i].quantity);
                int qtyWidth = MeasureText(qtyText, 12);
                DrawText(qtyText, badgeX + (badgeW - qtyWidth)/2, badgeY + 2, 12, GOLD);
            }
        }
    }

    // hover to show item name
    Vector2 m = GetMousePosition();
    for (int i = 0; i < INVENTORY_SLOTS; i++) {
        int x = barX + i*(slotSize + slotMargin);
        int y = barY + slotMargin;
        Rectangle r = { x, y, slotSize, slotSize };
        if (CheckCollisionPointRec(m, r) && inventory.slots[i].type != ITEM_NONE) {
            const char *label = GetItemName(inventory.slots[i].type); // implement this
            Vector2 sz = MeasureTextEx(GetFontDefault(), label, 16, 1);
            DrawRectangle(m.x, m.y - 24, sz.x + 8, 20, Fade(BLACK,0.6f));
            DrawText(label, m.x + 4, m.y - 22, 16, WHITE);
            break;
        }
    }
}

// Initialize shop
void InitShop() {
    shop.playerMoney = STARTING_MONEY;
    shop.x = SHOP_X;
    shop.y = SHOP_Y;
    world[shop.x][shop.y].objectType = OBJECT_TYPE_SHOP;
    shop.isPlayerInShop = false;
    shop.activeCategory = SHOP_CATEGORY_NONE;
    
    // Initialize prices
    for (int i = 1; i < CROP_COUNT; i++) {
        shop.seedPrices[i] = 1; // Base seed price
        
        // Base produce prices
        if (i == CROP_WHEAT) 
            shop.producePrices[i] = GetRandomValue(2, 3);
        else if (i == CROP_CORN || i == CROP_POTATO)
            shop.producePrices[i] = GetRandomValue(3, 5);
        else if (i == CROP_BERRY)
            shop.producePrices[i] = GetRandomValue(1,3);
    }
}

void GameStartup() {
    SetExitKey(0); // Prevent ESC from closing the window

    InitAudioDevice();

    sfxWalk = LoadSound("sounds/sfx_step_grass_l.wav");
    sfxPlow = LoadSound("sounds/hoe.wav");
    sfxPlant = LoadSound("sounds/impacts_plants.wav");
    sfxWater = LoadSound("sounds/Watering Outdoor Plants _ HQ Sound Effect.wav");
    sfxHarvest = LoadSound("sounds/harvest-plant.wav");

    SetSoundVolume(sfxPlow, 0.3f);
    SetSoundVolume(sfxWalk, 0.5f);

    sfxBuy = LoadSound("sounds/falling-coins.wav");
    sfxSell = LoadSound("sounds/getting-coins.wav");
    sfxClick = LoadSound("sounds/click_sound.wav");

    bgMusic = LoadMusicStream("sounds/Settlers of Catan Music.mp3");

    PlayMusicStream(bgMusic);
    SetMusicVolume(bgMusic, 0.1f);

    InitTimeSystem(&gameTime);

    Image image = LoadImage("textures/spritesheet.png");
    textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
    UnloadImage(image);

    InitWorld();

    player = (sEntity)
    {
        .x = TILE_WIDTH * 3,
        .y = TILE_HEIGHT * 3
    };

    camera.target = Vector2Lerp (camera.target, (Vector2){(player.x), player.y}, 0.1f);
    camera.offset = (Vector2){(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 4.0f; 

    // Inventory 
    InitInventory();

    InitShop();
}



void GameUpdate(void) {
    camera.offset = (Vector2){(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };

    UpdateMusicStream(bgMusic);

    static float walkSfxTimer = 0.0f;
    const float walkSfxInterval = 0.4f; // seconds between steps

    Vector2 dir = {0};
    if (IsKeyDown(KEY_A)|IsKeyDown(KEY_LEFT))  dir.x = -1;
    if (IsKeyDown(KEY_D)|IsKeyDown(KEY_RIGHT)) dir.x = +1;
    if (IsKeyDown(KEY_W)|IsKeyDown(KEY_UP))    dir.y = -1;
    if (IsKeyDown(KEY_S)|IsKeyDown(KEY_DOWN))  dir.y = +1;

    if (dir.x != 0 || dir.y != 0) {
        walkSfxTimer -= GetFrameTime();
        
        float moveSpeed = 2.0f;
        Vector2 nextPos = {
            player.x + dir.x * moveSpeed,
            player.y + dir.y * moveSpeed
        };

        // Calculate the tile the player would move into
        int destTileX = (int)((nextPos.x + TILE_WIDTH/2) / TILE_WIDTH);
        int destTileY = (int)((nextPos.y + TILE_HEIGHT/2) / TILE_HEIGHT);

        // Only move if not out of bounds and not blocked
        if (!IsOutOfBounds(nextPos.x, nextPos.y) && !IsBlockedByObject(destTileX, destTileY)) {
            player.x = nextPos.x;
            player.y = nextPos.y;
            if (walkSfxTimer <= 0.0f) {
                PlaySound(sfxWalk);
                walkSfxTimer = walkSfxInterval;
            }
        }
        // else: do not move at all if blocked
    } else {
        walkSfxTimer = 0.0f; // reset timer when not moving
    }
    
    // Camera uses the exact player position
    camera.target.x = player.x;
    camera.target.y = player.y;
    
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        const float zoomIncrement = 0.125f;
        camera.zoom += (wheel * zoomIncrement);
        if (camera.zoom < 2.0f) camera.zoom = 2.0f;
        if (camera.zoom > 4.0f) camera.zoom = 4.0f;
    }

    UpdateTimeSystem(&gameTime, GetFrameTime());
    UpdateDayLight  (&gameTime, GetFrameTime());

    // Debug controls for time speed
    if (IsKeyPressed(KEY_P)) gameTime.timeSpeedMultiplier *= 2.0f;
    if (IsKeyPressed(KEY_O)) gameTime.timeSpeedMultiplier *= 0.5f;
    if (IsKeyPressed(KEY_L)) gameTime.timeSpeedMultiplier = 1.0f;

    UpdateCropsByDay(&gameTime);

    UpdateInventory();

    // Checks if player is near shop
    float playerTileX = (player.x / TILE_WIDTH);
    float playerTileY = (player.y / TILE_HEIGHT);

    float dx = playerTileX - shop.x;    
    float dy = playerTileY - shop.y;
    float distSquared = dx*dx + dy*dy;

    shop.isPlayerInShop = (distSquared <= 2.25f);
    
    // Handle shop interaction
    if (shop.isPlayerInShop && IsKeyPressed(KEY_SPACE)) {
        shop.activeCategory = SHOP_CATEGORY_SEEDS;
    } 
    else if (IsKeyPressed(KEY_ESCAPE)) {
        if(shop.activeCategory != SHOP_CATEGORY_NONE) {
            shop.activeCategory = SHOP_CATEGORY_NONE;
        } else {
            gameState = GAME_STATE_PAUSED;
        }
    }
    // auto‐close if player walks away
    else if (!shop.isPlayerInShop) {
        shop.activeCategory = SHOP_CATEGORY_NONE;
    }
}
void GameRender() {

    BeginMode2D(camera);

    sTile tile;

    for (int i = 0; i < WORLD_WIDTH; i++) {
        for (int j = 0; j < WORLD_HEIGHT; j++) {
            tile = world[i][j];

            DrawTile(
                tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT,
                GetBaseTextureX(tile.baseType),
                GetBaseTextureY(tile.baseType)
            );

            if(tile.objectType != OBJECT_TYPE_NONE) {
                DrawTile(
                    tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT,
                    GetObjectTextureX(tile.objectType),
                    GetObjectTextureY(tile.objectType)
                );
            }
        }
    }

    // Draw crops
    for(int i = 0; i < MAX_CROPS; i++) {
        if(crops[i].type != CROP_NONE) {
            int textureX, textureY;

            switch(crops[i].type) {
                case CROP_WHEAT:
                    textureX = 4 - crops[i].growthStage;
                    textureY = 11;
                break;
                case CROP_POTATO:
                    textureX = 4 - crops[i].growthStage;
                    textureY = 13;
                break;
                case CROP_CORN:
                    textureX = 4 - crops[i].growthStage;
                    textureY = 15;
                break;
                case CROP_BERRY:
                    textureX = 10 - crops[i].growthStage;
                    textureY = 13;
                default: break;
            }
            DrawTile(
                crops[i].position.x * TILE_WIDTH,
                crops[i].position.y * TILE_HEIGHT,
                textureX, textureY
            );
        }
    }

    // player render
    DrawTile((int)player.x, (int)player.y, 6, 0);


    EndMode2D();

    // day/night cycle
    if(gameTime.currentLight < 0.99f) {
        float nightIntensity = 1.0f - gameTime.currentLight;
        Color nightColor = gameTime.nightColor;
        nightColor.a = (unsigned char)(220 * nightIntensity);

        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
            Fade(nightColor, nightIntensity * 0.8f)
        );

    }

    DrawShop();

    char timeText[50];
    snprintf(timeText, sizeof(timeText), "Year %d, Day %d \n%s  %02d:%02d x%.1f",
        gameTime.year,
        gameTime.day,
        GetSeasonName(gameTime.season),
        gameTime.hour,
        gameTime.minute,
        gameTime.timeSpeedMultiplier
    );

    DrawText(timeText, 10, 10, 20, BLACK);

    // hover tooltip: show water level if a crop is under the cursor
    Vector2 mouseScreen = GetMousePosition();
    Vector2 mouseWorld  = GetScreenToWorld2D(mouseScreen, camera);
    int hoverX = (int)(mouseWorld.x / TILE_WIDTH);
    int hoverY = (int)(mouseWorld.y / TILE_HEIGHT);
    for (int i = 0; i < MAX_CROPS; i++) {
        if (crops[i].type != CROP_NONE &&
            crops[i].position.x == hoverX &&
            crops[i].position.y == hoverY) {
            DrawText(
                TextFormat("Water: %d%%", crops[i].waterLvl),
                mouseScreen.x + 12, mouseScreen.y + 12,
                14, BLUE
            );
            break;
        }
    }

    // inventory UI
    DrawInventory();

    // Draw money above inventory
    int moneyY = GetScreenHeight() - 90;
    char moneyText[32];
    snprintf(moneyText, sizeof(moneyText), "%d", shop.playerMoney);
    int textWidth = MeasureText(moneyText, 28);

    int moneyDisplayWidth = 32 + 8 + textWidth;
    int moneyX = GetScreenWidth()/2 - moneyDisplayWidth/2;
    // Draw gold icon (adjust sprite coords if needed)
    static Texture2D coinIcon = {0};
    if (coinIcon.id == 0) {
        // coin coord in spritesheet
        coinIcon = CreateIconFromSpritesheet(textures[TEXTURE_TILEMAP], 3, 17);
    }
    DrawTextureEx(coinIcon, (Vector2){moneyX, moneyY}, 0, 1.5f, GOLD);

    // Draw money text next to icon
    DrawText(moneyText, moneyX + 40, moneyY, 28, GOLD);

}

void GameShutDown() {
    for(int i = 0; i < INVENTORY_SLOTS; i++) {
        if(inventory.slots[i].texture.id != 0) {
            UnloadTexture(inventory.slots[i].texture);
            inventory.slots[i].texture = (Texture2D){0};
        }
    }
    
    for(int i = 0; i < MAX_TEXTURES; i++) {
        UnloadTexture(textures[i]);
    }

    UnloadSound(sfxWalk);
    UnloadSound(sfxPlow);
    UnloadSound(sfxPlant);
    UnloadSound(sfxWater);
    UnloadSound(sfxHarvest);
    UnloadSound(sfxBuy);
    UnloadSound(sfxSell);
    UnloadSound(sfxClick);

    CloseAudioDevice();
}