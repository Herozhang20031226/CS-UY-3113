#include "Crop.h"

Texture2D Crop::mSpriteSheet = { 0 };
bool Crop::mSpriteSheetLoaded = false;

const CropInfo Crop::CROP_DATA[3] = {
    { "Beet",10.0f, 50, {8, 9, 10, 11, 12,13 ,15} },
    { "Pumpkin", 20.0f, 80, {26, 27, 28, 29, 31} },
    { "Grape",30.0f, 100, {40,42,43,44,45,47} }
};

void Crop::loadSpriteSheet(const char* filepath)
{
    if (!mSpriteSheetLoaded)
    {
        mSpriteSheet = LoadTexture(filepath);
        mSpriteSheetLoaded = true;
        }
    }


void Crop::unloadSpriteSheet()
{
    if (mSpriteSheetLoaded)
    {
        UnloadTexture(mSpriteSheet);
        mSpriteSheetLoaded = false;
    }
}

Crop::Crop()
    : mType(beet),
      mState(empty),
      mGrowthTimer(0.0f),
      mGrowthDuration(0.0f),
      mPosition{0.0f, 0.0f},
      mTilePosition{0.0f, 0.0f},
      mGoldValue(0),
      mCurrentFrameIndex(0)
{
}

Crop::Crop(CropType type, Vector2 worldPosition, Vector2 tilePosition)
    : mType(type),
      mState(grow),
      mGrowthTimer(0.0f),
      mPosition(worldPosition),
      mTilePosition(tilePosition),
      mCurrentFrameIndex(0)
{
    mGrowthDuration = CROP_DATA[type].growthTime;
    mGoldValue      = CROP_DATA[type].goldValue;
    
}

void Crop::updateFrameIndex()
{
    const std::vector<int>& frames = CROP_DATA[mType].frameIndices;
    int numFrames = (int)frames.size();
    
    float progress = getGrowthProgress();

    if (progress >= 1.0f)
    {
        mCurrentFrameIndex = numFrames - 1;
        mState = done;
    }
    else
    {
        mCurrentFrameIndex = (int)(progress * numFrames);
        if (mCurrentFrameIndex >= numFrames) 
            mCurrentFrameIndex = numFrames - 1;
        
        mState = grow;
    }
}

void Crop::update(float deltaTime)
{
    if (mState == empty) return;

    mGrowthTimer += deltaTime;
    updateFrameIndex();
}

void Crop::render(float tileSize)
{
    if (mState == empty) return;
    
    if (!mSpriteSheetLoaded || mSpriteSheet.id == 0)
    {
        Color colors[3] = { RED, ORANGE, PURPLE };
        float displaySize = tileSize * 0.8f;
        
        DrawRectangle(
            (int)(mPosition.x - displaySize/2),
            (int)(mPosition.y - displaySize/2),
            (int)displaySize,
            (int)displaySize,
            colors[mType]
        );
        return;
    }

    const std::vector<int>& frames = CROP_DATA[mType].frameIndices;
    int actualFrame = frames[mCurrentFrameIndex];
    
    int row = actualFrame / SHEET_WIDTH;
    int col = actualFrame % SHEET_WIDTH;
    
    Rectangle sourceRect = {
        (float)(col * SPRITE_SIZE),
        (float)(row * SPRITE_SIZE),
        (float)SPRITE_SIZE,
        (float)SPRITE_SIZE
    };

    float displaySize = tileSize * 0.8f;
    
    Rectangle destRect = {
        mPosition.x - displaySize/2,
        mPosition.y - displaySize/2,
        displaySize,
        displaySize
    };

    DrawTexturePro(
        mSpriteSheet,
        sourceRect,
        destRect,
        {0, 0},
        0.0f,
        WHITE
    );
    
}

float Crop::getGrowthProgress() const
{
    if (mGrowthDuration <= 0.0f) return 0.0f;
    float p = mGrowthTimer / mGrowthDuration;
    if (p < 0.0f) p = 0.0f;
    if (p > 1.0f) p = 1.0f;
    return p;
}

const char* Crop::getName() const
{
    return CROP_DATA[mType].name;
}

void Crop::harvest()
{
    mState = empty;
    mGrowthTimer = 0.0f;
    mCurrentFrameIndex = 0;
}