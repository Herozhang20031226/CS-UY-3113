#ifndef CROP_H
#define CROP_H
#include "cs3113.h"

enum CropType {beet,pumpkin,grape};

enum CropState {empty,grow,done};

struct CropInfo {
    const char* name;
    float growthTime;
    int   goldValue;
    std::vector<int> frameIndices; 
};

class Crop {
private:
    CropType  mType;
    CropState mState;
    float     mGrowthTimer;
    float     mGrowthDuration;
    Vector2   mPosition;
    Vector2   mTilePosition;
    int       mGoldValue;
    int       mCurrentFrameIndex; 

    static Texture2D mSpriteSheet;
    static bool      mSpriteSheetLoaded;

    void updateFrameIndex();

public:
    static constexpr int SPRITE_SIZE = 16;   
    static constexpr int SHEET_WIDTH = 8;  

    static const CropInfo CROP_DATA[3];

    Crop();
    Crop(CropType type, Vector2 worldPosition, Vector2 tilePosition);
    
    void update(float deltaTime);
    void render(float tileSize);
    
    static void loadSpriteSheet(const char* filepath);
    static void unloadSpriteSheet();
    
    CropState getState()         const { return mState; }
    CropType  getType()          const { return mType; }
    int       getGoldValue()     const { return mGoldValue; }
    Vector2   getPosition()      const { return mPosition; }
    Vector2   getTilePosition()  const { return mTilePosition; }
    float     getGrowthProgress()const;
    const char* getName()        const;
    
    bool isMature() const { return mState == done; }
    bool isEmpty()  const { return mState == empty;}
    
    void harvest();
};

#endif // CROP_H
