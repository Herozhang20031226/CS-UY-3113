#ifndef LEVEL_B_H
#define LEVEL_B_H

#include "Scene.h"

struct UnitStats
{
    float maxHealth;
    float currentHealth;
    float attackDamage;
    float attackSpeed;      
    float moveSpeed;        
    float attackCooldown;   
};


enum BattleUnitType
{
    UNIT_ZOMBIE,
    UNIT_ENEMY
};

enum BattleUnitState {UNIT_IDLE, UNIT_WAITING,UNIT_MARCHING,UNIT_FIGHTING,UNIT_DEAD};

struct BattleUnit
{
    BattleUnitType type;
    BattleUnitState state;
    
    std::string name;
    Texture2D texture;
    bool textureLoaded;
    
    Vector2 position;
    Vector2 scale;
    Vector2 originalScale;  
    
    UnitStats stats;
    float originalDamage; 
    
    int queueIndex;    
    
    BattleUnit* currentTarget;  
};


class LevelB : public Scene
{
private:
    Texture2D mBackground;
    bool mBackgroundLoaded;
    
    std::vector<BattleUnit*> mZombies;      
    std::vector<BattleUnit*> mEnemies;       
    
    int mCurrentDeployIndex;                 
    float mDeploymentProgress;              
    float mDeploymentTime;                 
    bool mCanDeploy;                       
    
    float mQueueStartX;                     
    float mQueueSpacing;                    
    float mDeployPositionX;             
    float mBattleY;                    
    
    float mEnemySpawnX;                  
    int mCurrentEnemyWave;               
    float mEnemySpawnTimer;           
    float mEnemySpawnDelay;       
    bool mBossSpawned;            
    
    bool mBattleStarted;
    bool mBattleOver;
    bool mPlayerWon;
    
    int mSelectedOpponent;
    
    Texture2D mFarmhandTexture;
    Texture2D mLumberjackTexture;
    Texture2D mBossTexture;
    
    float mBattleTimer;                    
    bool mDoOrDieMode;                     
    Texture2D mDoOrDieTexture;            
    bool mDoOrDieTextureLoaded;
    float mDoOrDiePulseTimer;              
    float mDoOrDieScale;                   
    Music mDoOrDieMusic;                   
    bool mDoOrDieMusicLoaded;
    float mDoOrDieFlashTimer;             
    bool mDoOrDieJustActivated;            
    float mDoOrDieTransitionTimer;         
    float mDoOrDieTransitionDuration;     
    bool mDoOrDieTransitionComplete;      
    
    void loadAssets();
    void unloadAssets();
    
    void createZombieArmy();
    void createEnemyWave();
    void spawnEnemy(int enemyType);  
    
    void updateDeploymentBar(float deltaTime);
    void updateZombies(float deltaTime);
    void updateEnemies(float deltaTime);
    void updateCombat(float deltaTime);
    void checkBattleEnd();
    
    void activateDoOrDie();                
    void updateDoOrDie(float deltaTime);  
    void renderDoOrDie();               
    
    void renderBackground();
    void renderUnits();
    void renderHealthBars();
    void renderDeploymentBar();
    void renderUI();
    void renderBattleResult();
    
    bool checkCollision(BattleUnit* a, BattleUnit* b);
    void dealDamage(BattleUnit* attacker, BattleUnit* target, float deltaTime);
    
public:
    static constexpr float ZOMBIE_WIDTH = 80.0f;
    static constexpr float ZOMBIE_HEIGHT = 100.0f;
    static constexpr float ENEMY_WIDTH = 120.0f;
    static constexpr float ENEMY_HEIGHT = 150.0f;
    static constexpr float BOSS_WIDTH = 140.0f;
    static constexpr float BOSS_HEIGHT = 160.0f;
    
    static constexpr float BATTLE_Y = 600.0f;           
    static constexpr float QUEUE_START_X = 50.0f;       
    static constexpr float QUEUE_SPACING = 70.0f;       
    static constexpr float DEPLOY_POSITION_X = 150.0f;  
    static constexpr float ENEMY_SPAWN_X = 950.0f;      
    static constexpr float ENEMY_DEFEND_X = 750.0f;     
    
    static constexpr float DEPLOYMENT_TIME = 6.0f;      
    static constexpr float ENEMY_SPAWN_DELAY = 0.0f;  
    static constexpr float DO_OR_DIE_TIME = 80.0f;   
    
    static constexpr float HEALTH_BAR_WIDTH = 60.0f;
    static constexpr float HEALTH_BAR_HEIGHT = 8.0f;
    static constexpr float HEALTH_BAR_OFFSET_Y = 20.0f;
    
    LevelB();
    LevelB(Vector2 origin, const char* bgHexCode, int selectedOpponent = 0);
    ~LevelB();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
    
    void setSelectedOpponent(int opponent) { mSelectedOpponent = opponent; }
    void setZombieUpgraded(int index, bool upgraded);  
    bool isBattleOver() const { return mBattleOver; }
    bool didPlayerWin() const { return mPlayerWon; }
    
private:
    bool mZombieUpgraded[5] = { false, false, false, false, false };  
};

#endif // LEVEL_B_H