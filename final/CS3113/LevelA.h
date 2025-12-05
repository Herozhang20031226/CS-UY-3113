#ifndef LEVEL_A_H
#define LEVEL_A_H

#include "Scene.h"
#include "Crop.h"

constexpr int LEVEL_WIDTH  = 12;
constexpr int LEVEL_HEIGHT = 10;

class LevelA : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        9, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12,
        33, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 60,
        33, 58, 297, 299, 299, 299, 300, 58, 58, 58, 58, 60,
        33, 58, 321, 346, 346, 346, 348, 58, 58, 58, 58, 60,
        33, 58, 321, 346, 346, 346, 348, 58, 58, 58, 58, 60,
        33, 58, 321, 346, 346, 346, 348, 58, 58, 58, 58, 60,
        33, 58, 369, 370, 370, 370, 372, 58, 58, 58, 58, 60,
        33, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 60,
        33, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 60,
        81, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 84,
    };

    std::vector<Crop> mCrops;
    CropType mSelectedCropType;
    Entity* mCampfire;

    struct ArmySlot {
        std::string name;   
        int         cost;   
        bool        unlocked;
        int         level;  
        Rectangle   rect;   
    };

    std::vector<ArmySlot> mArmySlots;

    Rectangle mBarRect{};              
    Rectangle mPurchaseWindowRect{};   
    Rectangle mBuyButtonRect{};        
    Rectangle mCancelButtonRect{};   

    int  mSelectedSlotIndex    = -1;
    bool mIsPurchaseWindowOpen = false;

    bool   isSoilTile(int tileIndex);
    Vector2 getTilePosition(Vector2 worldPosition);
    Vector2 getWorldPositionFromTile(int row, int col);
    Crop*  getCropAtTile(int row, int col);
    bool   canPlantAt(int row, int col);
    void   plantCrop(CropType type, int row, int col);
    void   harvestCropAt(int row, int col);
    void   checkCropInteraction();
    void   checkCropCollisions();

    void initialiseArmyBarUI();
    void updateArmyBarUI(float deltaTime);
    void renderArmyBarUI();
    void handleSlotClick(Vector2 mousePos);
    void handlePurchaseClick(Vector2 mousePos);

public:
    static constexpr float TILE_DIMENSION           = 50.0f;
    static constexpr float ACCELERATION_OF_GRAVITY  = 981.0f;
    static constexpr float END_GAME_THRESHOLD       = 800.0f;
    static constexpr float INTERACTION_RANGE        = 60.0f;
    bool spendGold(int amount);  

    LevelA();
    LevelA(Vector2 origin, const char *bgHexCode);
    ~LevelA();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

    void renderUI();
};

#endif // LEVEL_A_H
