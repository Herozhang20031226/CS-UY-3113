#include "LevelA.h"

LevelA::LevelA() : Scene{{0.0f, 0.0f}, nullptr} {}

LevelA::LevelA(Vector2 origin, const char *bgHexCode)
    : Scene{origin, bgHexCode}
{
    mSelectedCropType = beet;
}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
    mGameState.bgm = LoadMusicStream("assets/game/farm.mp3");
    SetMusicVolume(mGameState.bgm, 0.33f);

    PlayMusicStream(mGameState.bgm);

    mGameState.nextSceneID = 0;
    mGameState.campfire = nullptr;

    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,
        (unsigned int *)mLevelData,
        "assets/game/Tileset Grass Summer.png",
        TILE_DIMENSION,
        24, 40,
        mOrigin
    );

    std::map<Direction, std::vector<int>> xochitlAnimationAtlas = {
        { DOWN,  { 0, 1, 2, 3, 4, 5, 6, 7 } },
        { UP,    { 8, 9,10,11,12,13,14,15 } },
        { RIGHT, {16,17,18,19,20,21,22,23 } },
        { LEFT,  { 0, 1, 2, 3, 4, 5, 6, 7 } },
    };

    mGameState.xochitl = new Entity(
        { mOrigin.x, mOrigin.y },
        { 64.0f, 64.0f },
        "assets/game/run.png",
        ATLAS,
        { 3, 8 },
        xochitlAnimationAtlas,
        PLAYER
    );

    mGameState.gold     = 0;
    mGameState.goldIcon = LoadTexture("assets/game/gold.png");

    mGameState.xochitl->setJumpingPower(550.0f);
    mGameState.xochitl->setColliderDimensions({
        mGameState.xochitl->getScale().x / 3.5f,
        mGameState.xochitl->getScale().y / 3.0f
    });
    mGameState.xochitl->setAcceleration({0.0f, 0.0f});

    mGameState.camera = { 0 };
    mGameState.camera.target   = mGameState.xochitl->getPosition();
    mGameState.camera.offset   = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom     = 1.0f;

    Crop::loadSpriteSheet("assets/game/Fall Crops.png");
    mSelectedCropType = beet;
    
    std::map<Direction, std::vector<int>> campfireAnimation = {
        { DOWN,  { 0, 1, 2, 3, 4, 5 } },
        { UP,    { 0, 1, 2, 3, 4, 5 } },
        { RIGHT, { 0, 1, 2, 3, 4, 5 } },
        { LEFT,  { 0, 1, 2, 3, 4, 5 } },
    };
    
    Vector2 campfirePos = getWorldPositionFromTile(4, 8);
    mGameState.campfire = new Entity(  
        campfirePos,                      
        { 48.0f, 96.0f },                 
        "assets/game/bonfire.png",        
        ATLAS,                           
        { 1, 6 },                       
        campfireAnimation,          
        DECORATION                     
    );
    mGameState.campfire->setFrameSpeed(8);          
}

bool LevelA::isSoilTile(int tileIndex)
{
    return (tileIndex == 346);
}

Vector2 LevelA::getTilePosition(Vector2 worldPosition)
{
    float leftBoundary = mOrigin.x - (LEVEL_WIDTH  * TILE_DIMENSION) / 2.0f;
    float topBoundary  = mOrigin.y - (LEVEL_HEIGHT * TILE_DIMENSION) / 2.0f;

    int col = (int)floor((worldPosition.x - leftBoundary) / TILE_DIMENSION);
    int row = (int)floor((worldPosition.y - topBoundary)  / TILE_DIMENSION);

    return { (float)row, (float)col };
}

Vector2 LevelA::getWorldPositionFromTile(int row, int col)
{
    float leftBoundary = mOrigin.x - (LEVEL_WIDTH  * TILE_DIMENSION) / 2.0f;
    float topBoundary  = mOrigin.y - (LEVEL_HEIGHT * TILE_DIMENSION) / 2.0f;

    float x = leftBoundary + col * TILE_DIMENSION + TILE_DIMENSION / 2.0f;
    float y = topBoundary  + row * TILE_DIMENSION + TILE_DIMENSION / 2.0f;

    return { x, y };
}

Crop* LevelA::getCropAtTile(int row, int col)
{
    for (auto &crop : mCrops)
    {
        Vector2 tilePos = crop.getTilePosition();
        if ((int)tilePos.x == row && (int)tilePos.y == col)
        {
            return &crop;
        }
    }
    return nullptr;
}

bool LevelA::canPlantAt(int row, int col)
{
    if (row < 0 || row >= LEVEL_HEIGHT || col < 0 || col >= LEVEL_WIDTH)
        return false;

    int tileIndex = mLevelData[row * LEVEL_WIDTH + col];
    if (!isSoilTile(tileIndex))
        return false;

    if (getCropAtTile(row, col) != nullptr)
        return false;

    return true;
}

void LevelA::plantCrop(CropType type, int row, int col)
{
    if (!canPlantAt(row, col))
        return;

    Vector2 worldPos = getWorldPositionFromTile(row, col);
    Vector2 tilePos  = { (float)row, (float)col };
    
    mCrops.push_back(Crop(type, worldPos, tilePos));
}

void LevelA::harvestCropAt(int row, int col)
{
    Crop *crop = getCropAtTile(row, col);
    if (crop != nullptr && crop->isMature())
    {
        mGameState.gold += crop->getGoldValue();

        for (auto it = mCrops.begin(); it != mCrops.end(); ++it)
        {
            Vector2 tilePos = it->getTilePosition();
            if ((int)tilePos.x == row && (int)tilePos.y == col)
            {
                mCrops.erase(it);
                break;
            }
        }
    }
}

void LevelA::checkCropCollisions()
{
    Vector2 playerPos = mGameState.xochitl->getPosition();
    
    for (auto it = mCrops.begin(); it != mCrops.end(); )
    {
        if (it->isMature())
        {
            Vector2 cropPos = it->getPosition();
            float distance = Vector2Distance(playerPos, cropPos);
            
            if (distance < 40.0f)
            {
                mGameState.gold += it->getGoldValue();
                it = mCrops.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void LevelA::checkCropInteraction()
{
    if (IsKeyPressed(KEY_E))
    {
        Vector2 playerPos  = mGameState.xochitl->getPosition();
        Vector2 playerTile = getTilePosition(playerPos);

        int playerRow = (int)playerTile.x;
        int playerCol = (int)playerTile.y;

        bool planted = false;
        
        for (int dr = -1; dr <= 1 && !planted; dr++)
        {
            for (int dc = -1; dc <= 1 && !planted; dc++)
            {
                int checkRow = playerRow + dr;
                int checkCol = playerCol + dc;
                
                if (canPlantAt(checkRow, checkCol))
                {
                    plantCrop(mSelectedCropType, checkRow, checkCol);
                    planted = true;
                }
            }
        }
    }
}

void LevelA::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    if (IsKeyPressed(KEY_ONE))   { 
        mSelectedCropType = beet;
    }
    else if (IsKeyPressed(KEY_TWO))   { 
        mSelectedCropType = pumpkin;
    }
    else if (IsKeyPressed(KEY_THREE)) { 
        mSelectedCropType = grape;
    }

    mGameState.xochitl->update(deltaTime, nullptr, mGameState.map, nullptr, 0);

    for (auto &crop : mCrops)
    {
        crop.update(deltaTime);
    }
    
    if (mGameState.campfire)
    {
        mGameState.campfire->update(deltaTime, nullptr, nullptr, nullptr, 0);
    }

    checkCropInteraction();
    checkCropCollisions();

    Vector2 currentPlayerPosition = mGameState.xochitl->getPosition();

    if (currentPlayerPosition.y > END_GAME_THRESHOLD)
        mGameState.nextSceneID = 1;

    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelA::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    if (mGameState.map) mGameState.map->render();

    for (auto &crop : mCrops)
    {
        crop.render(TILE_DIMENSION);
    }
    
    if (mGameState.campfire)
    {
        mGameState.campfire->render();
    }

    if (mGameState.xochitl) mGameState.xochitl->render();

    Vector2 playerPos = mGameState.xochitl->getPosition();

    const char* cropNames[] = { "Beet", "Pumpkin", "Grape" };

    DrawText(
        TextFormat("Selected: %s [1-3]", cropNames[mSelectedCropType]),
        (int)(playerPos.x - 100),
        (int)(playerPos.y - 100),
        20,
        WHITE
    );

    DrawText(
        "E to Plant | Walk to Harvest",
        (int)(playerPos.x - 110),
        (int)(playerPos.y - 70),
        16,
        LIGHTGRAY
    );
}

bool LevelA::spendGold(int amount)
{
    if (amount <= 0) return true;
    if (mGameState.gold < amount) return false;

    mGameState.gold -= amount;
    return true;
}

void LevelA::shutdown()
{
    if (mGameState.xochitl)  { delete mGameState.xochitl;  mGameState.xochitl  = nullptr; }
    if (mGameState.map)      { delete mGameState.map;      mGameState.map      = nullptr; }
    if (mGameState.campfire) { delete mGameState.campfire; mGameState.campfire = nullptr; }

    UnloadTexture(mGameState.goldIcon);
    Crop::unloadSpriteSheet();
    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);

    mCrops.clear();
}
