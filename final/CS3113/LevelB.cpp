#include "LevelB.h"

static const UnitStats ZOMBIE_BASE_STATS[5] = {
    { 600.0f, 600.0f, 5.0f, 1.0f, 60.0f, 0.0f },   
    { 200.0f, 200.0f, 20.0f, 1.8f, 120.0f, 0.0f },
    { 280.0f, 280.0f, 60.0f, 0.7f, 60.0f, 0.0f },
    { 300.0f, 300.0f, 30.0f, 1.3f, 90.0f, 0.0f },
    { 50.0f, 50.0f, 15.0f, 2.0f, 140.0f, 0.0f }
};

static const UnitStats ZOMBIE_UPGRADED_STATS[5] = {
    { 2000.0f, 2000.0f, 10.0f, 1.0f, 60.0f, 0.0f },
    { 500.0f, 500.0f, 55.0f, 1.8f, 120.0f, 0.0f },
    { 600.0f, 600.0f, 150.0f, 0.7f, 60.0f, 0.0f },    
    { 700.0f, 700.0f, 90.0f, 1.3f, 90.0f, 0.0f },
    { 200.0f, 200.0f, 50.0f, 2.5f, 140.0f, 0.0f }
};

static const UnitStats FARMHAND_STATS = 
    { 1000.0f, 1000.0f, 10.0f, 1.4f, 50.0f, 0.0f };

static const UnitStats LUMBERJACK_STATS = 
    { 1000.0f, 1000.0f, 15.0f, 1.0f, 50.0f, 0.0f };

static const UnitStats BOSS_STATS = 
    { 2000.0f, 2000.0f, 30.0f, 0.8f,50.0f, 0.0f };

static const UnitStats NINJA_BOY_STATS = 
    { 2000.0f, 2000.0f, 30.0f, 1.0f, 60.0f, 0.0f };

static const UnitStats NINJA_GIRL_STATS = 
    { 3000.0f, 3000.0f, 24.0f, 1.2f, 50.0f, 0.0f };

static const UnitStats NINJA_BOSS_STATS = 
    { 6000.0f, 6000.0f, 15.0f, 3.0f, 40.0f, 0.0f };


LevelB::LevelB() 
    : Scene({ 0.0f, 0.0f }, nullptr),
      mBackgroundLoaded(false),
      mCurrentDeployIndex(0),
      mDeploymentProgress(0.0f),
      mDeploymentTime(DEPLOYMENT_TIME),
      mCanDeploy(false),
      mQueueStartX(QUEUE_START_X),
      mQueueSpacing(QUEUE_SPACING),
      mDeployPositionX(DEPLOY_POSITION_X),
      mBattleY(BATTLE_Y),
      mEnemySpawnX(ENEMY_SPAWN_X),
      mCurrentEnemyWave(0),
      mEnemySpawnTimer(0.0f),
      mEnemySpawnDelay(ENEMY_SPAWN_DELAY),
      mBossSpawned(false),
      mBattleStarted(false),
      mBattleOver(false),
      mPlayerWon(false),
      mSelectedOpponent(0),
      mBattleTimer(DO_OR_DIE_TIME),
      mDoOrDieMode(false),
      mDoOrDieTextureLoaded(false),
      mDoOrDiePulseTimer(0.0f),
      mDoOrDieScale(1.0f),
      mDoOrDieMusicLoaded(false),
      mDoOrDieFlashTimer(0.0f),
      mDoOrDieJustActivated(false),
      mDoOrDieTransitionTimer(0.0f),
      mDoOrDieTransitionDuration(1.5f),
      mDoOrDieTransitionComplete(false)
{
}

LevelB::LevelB(Vector2 origin, const char* bgHexCode, int selectedOpponent)
    : Scene(origin, bgHexCode),
      mBackgroundLoaded(false),
      mCurrentDeployIndex(0),
      mDeploymentProgress(0.0f),
      mDeploymentTime(DEPLOYMENT_TIME),
      mCanDeploy(false),
      mQueueStartX(QUEUE_START_X),
      mQueueSpacing(QUEUE_SPACING),
      mDeployPositionX(DEPLOY_POSITION_X),
      mBattleY(BATTLE_Y),
      mEnemySpawnX(ENEMY_SPAWN_X),
      mCurrentEnemyWave(0),
      mEnemySpawnTimer(0.0f),
      mEnemySpawnDelay(ENEMY_SPAWN_DELAY),
      mBossSpawned(false),
      mBattleStarted(false),
      mBattleOver(false),
      mPlayerWon(false),
      mSelectedOpponent(selectedOpponent),
      mBattleTimer(DO_OR_DIE_TIME),
      mDoOrDieMode(false),
      mDoOrDieTextureLoaded(false),
      mDoOrDiePulseTimer(0.0f),
      mDoOrDieScale(1.0f),
      mDoOrDieMusicLoaded(false),
      mDoOrDieFlashTimer(0.0f),
      mDoOrDieJustActivated(false),
      mDoOrDieTransitionTimer(0.0f),
      mDoOrDieTransitionDuration(1.5f),
      mDoOrDieTransitionComplete(false)
{
}

LevelB::~LevelB()
{
    shutdown();
}

void LevelB::initialise()
{
    if (mSelectedOpponent == 0)
    {
        mGameState.bgm = LoadMusicStream("assets/game/1.wav");
    }
    else
    {
        mGameState.bgm = LoadMusicStream("assets/game/2.wav");
    }
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    mGameState.nextSceneID = -1; 
    mGameState.xochitl = nullptr; 
    mGameState.map = nullptr;
    mGameState.campfire = nullptr;
    
    loadAssets();
    createZombieArmy();
    
    mBattleStarted = true;
    mBattleOver = false;
    mPlayerWon = false;
    mCurrentDeployIndex = 0;
    mDeploymentProgress = 0.0f;
    mCanDeploy = false;
    mCurrentEnemyWave = 0;
    mBossSpawned = false;
    mEnemySpawnTimer = 1.0f;
    
    mBattleTimer = DO_OR_DIE_TIME;
    mDoOrDieMode = false;
    mDoOrDiePulseTimer = 0.0f;
    mDoOrDieScale = 1.0f;
    mDoOrDieFlashTimer = 0.0f;
    mDoOrDieJustActivated = false;
    mDoOrDieTransitionTimer = 0.0f;
    mDoOrDieTransitionComplete = false;
    
    mGameState.camera = { 0 };
    mGameState.camera.target = { 500.0f, 300.0f };
    mGameState.camera.offset = { 500.0f, 300.0f };
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;
}

void LevelB::loadAssets()
{
    if (mSelectedOpponent == 0)
    {
        mBackground = LoadTexture("assets/game/scene1.png");
    }
    else
    {
        mBackground = LoadTexture("assets/game/scene2.png");
    }
    mBackgroundLoaded = (mBackground.id != 0);
    
    if (mSelectedOpponent == 0)
    {
        mFarmhandTexture = LoadTexture("assets/game/Farmhand.png");
        mLumberjackTexture = LoadTexture("assets/game/Lumberjack.png");
        mBossTexture = LoadTexture("assets/game/Old_McDonnell.png");
    }
    else
    {
        mFarmhandTexture = LoadTexture("assets/game/boy.png");      
        mLumberjackTexture = LoadTexture("assets/game/girl.png");   
        mBossTexture = LoadTexture("assets/game/boss.png");         
    }
    
    mDoOrDieTexture = LoadTexture("assets/game/Do_or_Die.png");
    mDoOrDieTextureLoaded = (mDoOrDieTexture.id != 0);
    
    mDoOrDieMusic = LoadMusicStream("assets/game/do_or_die.wav");
    mDoOrDieMusicLoaded = (mDoOrDieMusic.ctxData != nullptr);
    
    if (mDoOrDieMusicLoaded)
    {
        SetMusicVolume(mDoOrDieMusic, 0.5f);
    }
}

void LevelB::unloadAssets()
{
    if (mBackgroundLoaded && mBackground.id != 0)
    {
        UnloadTexture(mBackground);
        mBackground.id = 0;
        mBackgroundLoaded = false;
    }
    
    if (mFarmhandTexture.id != 0) 
    {
        UnloadTexture(mFarmhandTexture);
        mFarmhandTexture.id = 0;
    }
    if (mLumberjackTexture.id != 0) 
    {
        UnloadTexture(mLumberjackTexture);
        mLumberjackTexture.id = 0;
    }
    if (mBossTexture.id != 0) 
    {
        UnloadTexture(mBossTexture);
        mBossTexture.id = 0;
    }
    
    if (mDoOrDieTextureLoaded && mDoOrDieTexture.id != 0)
    {
        UnloadTexture(mDoOrDieTexture);
        mDoOrDieTexture.id = 0;
        mDoOrDieTextureLoaded = false;
    }
    
    if (mDoOrDieMusicLoaded)
    {
        UnloadMusicStream(mDoOrDieMusic);
        mDoOrDieMusicLoaded = false;
    }
}

void LevelB::setZombieUpgraded(int index, bool upgraded)
{
    if (index >= 0 && index < 5)
    {
        mZombieUpgraded[index] = upgraded;
    }
}

void LevelB::createZombieArmy()
{
    for (auto* zombie : mZombies)
    {
        if (zombie->textureLoaded && zombie->texture.id != 0)
        {
            UnloadTexture(zombie->texture);
        }
        delete zombie;
    }
    mZombies.clear();
    
    for (auto* enemy : mEnemies)
    {
        delete enemy;
    }
    mEnemies.clear();

    const char* zombieTextures[5] = {
        "assets/game/1.png",
        "assets/game/2.png",
        "assets/game/3.png",
        "assets/game/4.png",
        "assets/game/5.png"
    };
    
    const char* upgradedTextures[5] = {
        "assets/game/6.png",
        "assets/game/7.png",
        "assets/game/8.png",
        "assets/game/9.png",
        "assets/game/10.png"
    };
    
    const char* zombieNames[5] = {
        "Zombie I",
        "Zombie II", 
        "Zombie III",
        "Zombie IV",
        "Zombie V"
    };
    
    const Vector2 zombieSizes[5] = {
        { 150.0f, 200.0f },   
        { 150.0f, 200.0f },   
        { 200.0f, 250.0f },  
        { 150.0f, 200.0f },   
        { 90.0f, 110.0f }  
    };
    
    const Vector2 upgradedSizes[5] = {
        { 150.0f, 200.0f },   
        { 150.0f, 200.0f },   
        { 200.0f, 250.0f },  
        { 150.0f, 200.0f },   
        { 90.0f, 110.0f }  
    };
    
    for (int i = 0; i < 5; i++)
    {
        BattleUnit* zombie = new BattleUnit();
        
        zombie->type = UNIT_ZOMBIE;
        zombie->state = (i == 0) ? UNIT_WAITING : UNIT_IDLE;
        zombie->name = zombieNames[i];
        
        if (mZombieUpgraded[i])
        {
            zombie->texture = LoadTexture(upgradedTextures[i]);
            zombie->scale = upgradedSizes[i];
            zombie->stats = ZOMBIE_UPGRADED_STATS[i];
        }
        else
        {
            zombie->texture = LoadTexture(zombieTextures[i]);
            zombie->scale = zombieSizes[i];
            zombie->stats = ZOMBIE_BASE_STATS[i];
        }
        
        zombie->textureLoaded = (zombie->texture.id != 0);
        zombie->originalScale = zombie->scale;  // Store original scale
        zombie->originalDamage = zombie->stats.attackDamage;  // Store original damage
        
        zombie->queueIndex = i;
        
        if (i == 0)
        {
            zombie->position = { mDeployPositionX, mBattleY };
        }
        else
        {
            zombie->position = { 
                mQueueStartX + (4 - i) * mQueueSpacing, 
                mBattleY 
            };
        }
        
        zombie->currentTarget = nullptr;
        
        mZombies.push_back(zombie);
    }
}

void LevelB::spawnEnemy(int enemyType)
{
    BattleUnit* enemy = new BattleUnit();
    
    enemy->type = UNIT_ENEMY;
    enemy->state = UNIT_IDLE;
    enemy->currentTarget = nullptr;
    enemy->textureLoaded = false;
    enemy->queueIndex = (int)mEnemies.size();
    
    if (mSelectedOpponent == 0)
    {
        switch (enemyType)
        {
            case 0: 
                enemy->name = "Farmhand";
                enemy->texture = mFarmhandTexture;
                enemy->textureLoaded = (mFarmhandTexture.id != 0);
                enemy->scale = { ENEMY_WIDTH, ENEMY_HEIGHT };
                enemy->stats = FARMHAND_STATS;
                break;
                
            case 1: 
                enemy->name = "Lumberjack";
                enemy->texture = mLumberjackTexture;
                enemy->textureLoaded = (mLumberjackTexture.id != 0);
                enemy->scale = { ENEMY_WIDTH, ENEMY_HEIGHT };
                enemy->stats = LUMBERJACK_STATS;
                break;
                
            case 2:
                enemy->name = "Old McDonnell";
                enemy->texture = mBossTexture;
                enemy->textureLoaded = (mBossTexture.id != 0);
                enemy->scale = { BOSS_WIDTH, BOSS_HEIGHT };
                enemy->stats = BOSS_STATS;
                mBossSpawned = true;
                break;
        }
    }
    else
    {
        switch (enemyType)
        {
            case 0:
                enemy->name = "Ninja Boy";
                enemy->texture = mFarmhandTexture;
                enemy->textureLoaded = (mFarmhandTexture.id != 0);
                enemy->scale = { ENEMY_WIDTH-20, ENEMY_HEIGHT };
                enemy->stats = NINJA_BOY_STATS;
                break;
                
            case 1:
                enemy->name = "Ninja Girl";
                enemy->texture = mLumberjackTexture;
                enemy->textureLoaded = (mLumberjackTexture.id != 0);
                enemy->scale = { ENEMY_WIDTH, ENEMY_HEIGHT };
                enemy->stats = NINJA_GIRL_STATS;
                break;
                
            case 2:
                enemy->name = "Ninja Boss";
                enemy->texture = mBossTexture;
                enemy->textureLoaded = (mBossTexture.id != 0);
                enemy->scale = { BOSS_WIDTH, BOSS_HEIGHT + 20.0f };
                enemy->stats = NINJA_BOSS_STATS;
                mBossSpawned = true;
                break;
        }
    }
    
    enemy->position = { mEnemySpawnX, mBattleY };
    enemy->originalScale = enemy->scale;  
    enemy->originalDamage = enemy->stats.attackDamage; 
    
    mEnemies.push_back(enemy);
    
    if (mEnemies.size() == 1)
    {
        enemy->state = UNIT_MARCHING;
    }
}

void LevelB::activateDoOrDie()
{
    if (mDoOrDieMode) return; 
    
    mDoOrDieMode = true;
    mDoOrDieJustActivated = true;
    mDoOrDieFlashTimer = 2.0f;  
    mDoOrDiePulseTimer = 0.0f;
    mDoOrDieScale = 2.0f; 
    
    mDoOrDieTransitionTimer = 0.0f;
    mDoOrDieTransitionComplete = false;
        
    StopMusicStream(mGameState.bgm);
    if (mDoOrDieMusicLoaded)
    {
        PlayMusicStream(mDoOrDieMusic);
    }
    
    for (auto* enemy : mEnemies)
    {
        if (enemy->state != UNIT_DEAD)
        {
            enemy->stats.attackDamage = enemy->originalDamage * 2.0f;
        }
    }
}

void LevelB::updateDoOrDie(float deltaTime)
{
    if (!mDoOrDieMode) return;
    
    if (mDoOrDieMusicLoaded)
    {
        UpdateMusicStream(mDoOrDieMusic);
    }
    
    if (mDoOrDieFlashTimer > 0.0f)
    {
        mDoOrDieFlashTimer -= deltaTime;
        if (mDoOrDieFlashTimer <= 0.0f)
        {
            mDoOrDieJustActivated = false;
        }
    }
    
    if (!mDoOrDieTransitionComplete)
    {
        mDoOrDieTransitionTimer += deltaTime;
        
        float progress = mDoOrDieTransitionTimer / mDoOrDieTransitionDuration;
        
        if (progress >= 1.0f)
        {
            progress = 1.0f;
            mDoOrDieTransitionComplete = true;
        }
        
        float easedProgress = 1.0f - powf(1.0f - progress, 3.0f);
        float currentScaleMultiplier = 1.0f + (0.3f * easedProgress);
        
        for (auto* enemy : mEnemies)
        {
            if (enemy->state != UNIT_DEAD)
            {
                enemy->scale.x = enemy->originalScale.x * currentScaleMultiplier;
                enemy->scale.y = enemy->originalScale.y * currentScaleMultiplier;
            }
        }
    }
    
    mDoOrDiePulseTimer += deltaTime * 6.0f;  
    mDoOrDieScale = 0.8f + 0.2f * sinf(mDoOrDiePulseTimer); 
}

void LevelB::renderDoOrDie()
{
    if (!mDoOrDieMode) return;
    
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    
    float barHeight = 150.0f;  
    float barY = screenH / 2.0f - barHeight / 2.0f - 30.0f; 
    
    float alpha = 0.7f;
    if (mDoOrDieJustActivated)
    {
        alpha = 0.5f + 0.4f * sinf(mDoOrDieFlashTimer * 10.0f);
    }
    
    DrawRectangle(0, (int)barY, screenW, (int)barHeight, Fade(BLACK, alpha));
    
    if (mDoOrDieTextureLoaded)
    {
        float imgWidth = (float)mDoOrDieTexture.width * mDoOrDieScale * 1.8f;   // Was 2.0f, now smaller
        float imgHeight = (float)mDoOrDieTexture.height * mDoOrDieScale * 1.8f; // Was 2.0f, now smaller
        
        Rectangle src = { 0, 0, (float)mDoOrDieTexture.width, (float)mDoOrDieTexture.height };
        Rectangle dest = {
            screenW / 2.0f - imgWidth / 2.0f,
            barY + barHeight / 2.0f - imgHeight / 2.0f,
            imgWidth,
            imgHeight
        };
        
        DrawTexturePro(mDoOrDieTexture, src, dest, { 0, 0 }, 0.0f, WHITE);
    }
    else
    {
        const char* text = "DO OR DIE!";
        int fontSize = (int)(48 * mDoOrDieScale);
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, screenW / 2 - textWidth / 2, (int)(barY + barHeight / 2 - fontSize / 2), fontSize, RED);
    }
}

void LevelB::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    if (mBattleOver) 
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            mGameState.nextSceneID = 0;  
        }
        return;
    }
    
    if (!mDoOrDieMode && mBattleStarted)
    {
        mBattleTimer -= deltaTime;
        
        if (mBattleTimer <= 0.0f)
        {
            mBattleTimer = 0.0f;
            activateDoOrDie();
        }
    }
    
    updateDoOrDie(deltaTime);
    
    mEnemySpawnTimer -= deltaTime;
    
    if (mEnemySpawnTimer <= 0.0f && !mBossSpawned)
    {
        if (mCurrentEnemyWave == 0)
        {
            spawnEnemy(0);  
            mEnemySpawnTimer = mEnemySpawnDelay;
            mCurrentEnemyWave++;
        }
        else if (mCurrentEnemyWave == 1)
        {
            spawnEnemy(1);
            mEnemySpawnTimer = mEnemySpawnDelay;
            mCurrentEnemyWave++;
        }
        else if (mCurrentEnemyWave == 2)
        {
            spawnEnemy(2);
            mCurrentEnemyWave++;
        }
    }
    
    updateDeploymentBar(deltaTime);
    updateZombies(deltaTime);
    updateEnemies(deltaTime);
    updateCombat(deltaTime);
    checkBattleEnd();
}

void LevelB::updateDeploymentBar(float deltaTime)
{
    if (mCurrentDeployIndex >= (int)mZombies.size()) return;
    
    BattleUnit* currentZombie = mZombies[mCurrentDeployIndex];
    
    if (currentZombie->state != UNIT_WAITING) return;
    
    mDeploymentProgress += deltaTime / mDeploymentTime;
    
    if (mDeploymentProgress >= 1.0f)
    {
        mDeploymentProgress = 1.0f;
        mCanDeploy = true;
    }
    
    if (mCanDeploy && IsKeyPressed(KEY_SPACE))
    {
        currentZombie->state = UNIT_MARCHING;
                        
        mDeploymentProgress = 0.0f;
        mCanDeploy = false;
        
        mCurrentDeployIndex++;
        
        if (mCurrentDeployIndex < (int)mZombies.size())
        {
            mZombies[mCurrentDeployIndex]->state = UNIT_WAITING;
            mZombies[mCurrentDeployIndex]->position = { mDeployPositionX, mBattleY };
            
            for (int i = mCurrentDeployIndex + 1; i < (int)mZombies.size(); i++)
            {
                if (mZombies[i]->state == UNIT_IDLE)
                {
                    int queuePos = i - mCurrentDeployIndex - 1;
                    mZombies[i]->position.x = mQueueStartX + (3 - queuePos) * mQueueSpacing;
                }
            }
        }
    }
}

void LevelB::updateZombies(float deltaTime)
{
    for (auto* zombie : mZombies)
    {
        if (zombie->state == UNIT_DEAD) continue;
        
        if (zombie->state == UNIT_MARCHING)
        {
            zombie->position.x += zombie->stats.moveSpeed * deltaTime;
            
            for (auto* enemy : mEnemies)
            {
                if (enemy->state == UNIT_DEAD) continue;
                
                if (checkCollision(zombie, enemy))
                {
                    zombie->state = UNIT_FIGHTING;
                    zombie->currentTarget = enemy;
                    break;
                }
            }
        }
        else if (zombie->state == UNIT_FIGHTING)
        {
            if (zombie->currentTarget == nullptr || 
                zombie->currentTarget->state == UNIT_DEAD)
            {
                zombie->currentTarget = nullptr;
                zombie->state = UNIT_MARCHING;
                
                for (auto* enemy : mEnemies)
                {
                    if (enemy->state != UNIT_DEAD && checkCollision(zombie, enemy))
                    {
                        zombie->state = UNIT_FIGHTING;
                        zombie->currentTarget = enemy;
                        break;
                    }
                }
            }
        }
        
        if (zombie->stats.attackCooldown > 0.0f)
        {
            zombie->stats.attackCooldown -= deltaTime;
        }
    }
}

void LevelB::updateEnemies(float deltaTime)
{
    BattleUnit* activeEnemy = nullptr;
    int activeIndex = -1;
    
    for (int i = 0; i < (int)mEnemies.size(); i++)
    {
        BattleUnit* enemy = mEnemies[i];
        if (enemy->state == UNIT_MARCHING || 
            enemy->state == UNIT_WAITING || 
            enemy->state == UNIT_FIGHTING)
        {
            activeEnemy = enemy;
            activeIndex = i;
            break;
        }
    }
    
    if (activeEnemy == nullptr)
    {
        for (auto* enemy : mEnemies)
        {
            if (enemy->state == UNIT_IDLE)
            {
                enemy->state = UNIT_MARCHING;
                
                if (mDoOrDieMode)
                {
                    enemy->stats.attackDamage = enemy->originalDamage * 2.0f;
                    
                    if (mDoOrDieTransitionComplete)
                    {
                        enemy->scale.x = enemy->originalScale.x * 1.3f;
                        enemy->scale.y = enemy->originalScale.y * 1.3f;
                    }
                }
                break;
            }
        }
    }
    
    for (auto* enemy : mEnemies)
    {
        if (enemy->state == UNIT_DEAD || enemy->state == UNIT_IDLE) continue;
        
        if (enemy->state == UNIT_MARCHING)
        {
            if (enemy->position.x > ENEMY_DEFEND_X)
            {
                enemy->position.x -= enemy->stats.moveSpeed * deltaTime;
                
                if (enemy->position.x <= ENEMY_DEFEND_X)
                {
                    enemy->position.x = ENEMY_DEFEND_X;
                    enemy->state = UNIT_WAITING;
                }
            }
            
            for (auto* zombie : mZombies)
            {
                if (zombie->state == UNIT_DEAD || 
                    zombie->state == UNIT_IDLE || 
                    zombie->state == UNIT_WAITING) continue;
                
                if (checkCollision(enemy, zombie))
                {
                    enemy->state = UNIT_FIGHTING;
                    enemy->currentTarget = zombie;
                    break;
                }
            }
        }
        else if (enemy->state == UNIT_WAITING)
        {
            for (auto* zombie : mZombies)
            {
                if (zombie->state == UNIT_DEAD || 
                    zombie->state == UNIT_IDLE || 
                    zombie->state == UNIT_WAITING) continue;
                
                if (checkCollision(enemy, zombie))
                {
                    enemy->state = UNIT_FIGHTING;
                    enemy->currentTarget = zombie;
                    break;
                }
            }
        }
        else if (enemy->state == UNIT_FIGHTING)
        {
            if (enemy->currentTarget == nullptr || 
                enemy->currentTarget->state == UNIT_DEAD)
            {
                enemy->currentTarget = nullptr;
                enemy->state = UNIT_WAITING;
                
                for (auto* zombie : mZombies)
                {
                    if (zombie->state != UNIT_DEAD && 
                        zombie->state != UNIT_IDLE && 
                        zombie->state != UNIT_WAITING &&
                        checkCollision(enemy, zombie))
                    {
                        enemy->state = UNIT_FIGHTING;
                        enemy->currentTarget = zombie;
                        break;
                    }
                }
            }
        }
        
        if (enemy->stats.attackCooldown > 0.0f)
        {
            enemy->stats.attackCooldown -= deltaTime;
        }
    }
}

void LevelB::updateCombat(float deltaTime)
{
    for (auto* zombie : mZombies)
    {
        if (zombie->state == UNIT_FIGHTING && zombie->currentTarget != nullptr)
        {
            dealDamage(zombie, zombie->currentTarget, deltaTime);
        }
    }
    
    for (auto* enemy : mEnemies)
    {
        if (enemy->state == UNIT_FIGHTING && enemy->currentTarget != nullptr)
        {
            dealDamage(enemy, enemy->currentTarget, deltaTime);
        }
    }
    
    for (auto* zombie : mZombies)
    {
        if (zombie->stats.currentHealth <= 0.0f && zombie->state != UNIT_DEAD)
        {
            zombie->state = UNIT_DEAD;
            zombie->currentTarget = nullptr;
        }
    }
    
    for (auto* enemy : mEnemies)
    {
        if (enemy->stats.currentHealth <= 0.0f && enemy->state != UNIT_DEAD)
        {
            enemy->state = UNIT_DEAD;
            enemy->currentTarget = nullptr;
        }
    }
}

bool LevelB::checkCollision(BattleUnit* a, BattleUnit* b)
{
    float aLeft = a->position.x - a->scale.x / 2.0f;
    float aRight = a->position.x + a->scale.x / 2.0f;
    float bLeft = b->position.x - b->scale.x / 2.0f;
    float bRight = b->position.x + b->scale.x / 2.0f;
    
    return (aRight > bLeft && aLeft < bRight);
}

void LevelB::dealDamage(BattleUnit* attacker, BattleUnit* target, float deltaTime)
{
    if (attacker->stats.attackCooldown <= 0.0f)
    {
        target->stats.currentHealth -= attacker->stats.attackDamage;
        attacker->stats.attackCooldown = 1.0f / attacker->stats.attackSpeed;
    }
}

void LevelB::checkBattleEnd()
{
    bool allEnemiesDead = mBossSpawned;
    
    for (auto* enemy : mEnemies)
    {
        if (enemy->state != UNIT_DEAD)
        {
            allEnemiesDead = false;
            break;
        }
    }
    
    if (allEnemiesDead && mBossSpawned && !mEnemies.empty())
    {
        mBattleOver = true;
        mPlayerWon = true;
        return;
    }
    
    bool anyZombieAlive = false;
    
    for (auto* zombie : mZombies)
    {
        if (zombie->state != UNIT_DEAD)
        {
            anyZombieAlive = true;
            break;
        }
    }
    
    if (!anyZombieAlive)
    {
        bool anyEnemyAlive = false;
        for (auto* enemy : mEnemies)
        {
            if (enemy->state != UNIT_DEAD)
            {
                anyEnemyAlive = true;
                break;
            }
        }
        
        if (anyEnemyAlive)
        {
            mBattleOver = true;
            mPlayerWon = false;
        }
    }
}

void LevelB::render()
{
    renderBackground();
    renderUnits();
    renderHealthBars();
    renderDeploymentBar();
    renderUI();
    renderDoOrDie(); 
    
    if (mBattleOver)
    {
        renderBattleResult();
    }
}

void LevelB::renderBackground()
{
    if (mBackgroundLoaded)
    {
        Rectangle src = { 0, 0, (float)mBackground.width, (float)mBackground.height };
        Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
        
        DrawTexturePro(mBackground, src, dest, { 0, 0 }, 0.0f, WHITE);
    }
    else
    {
        ClearBackground(ColorFromHex("#87CEEB"));
        DrawRectangle(0, (int)mBattleY + 50, GetScreenWidth(), 
                      GetScreenHeight() - (int)mBattleY - 50, DARKGREEN);
    }
}

void LevelB::renderUnits()
{
    Color zombieTint = WHITE;
    Color enemyTint = mDoOrDieMode ? RED : WHITE;
    
    for (auto* zombie : mZombies)
    {
        if (zombie->state == UNIT_DEAD) continue;
        
        if (zombie->textureLoaded)
        {
            Rectangle src = { 0, 0, 
                -(float)zombie->texture.width, 
                (float)zombie->texture.height };
            
            Rectangle dest = {
                zombie->position.x - zombie->scale.x / 2.0f,
                zombie->position.y - zombie->scale.y,
                zombie->scale.x,
                zombie->scale.y
            };
            
            DrawTexturePro(zombie->texture, src, dest, { 0, 0 }, 0.0f, zombieTint);
        }
        else
        {
            DrawRectangle(
                (int)(zombie->position.x - zombie->scale.x / 2.0f),
                (int)(zombie->position.y - zombie->scale.y),
                (int)zombie->scale.x,
                (int)zombie->scale.y,
                GREEN
            );
        }
    }
    
    for (auto* enemy : mEnemies)
    {
        if (enemy->state == UNIT_DEAD) continue;
        
        if (enemy->textureLoaded && enemy->texture.id != 0)
        {
            Rectangle src = { 0, 0, 
                (float)enemy->texture.width, 
                (float)enemy->texture.height };
            
            Rectangle dest = {
                enemy->position.x - enemy->scale.x / 2.0f,
                enemy->position.y - enemy->scale.y,
                enemy->scale.x,
                enemy->scale.y
            };
            
            DrawTexturePro(enemy->texture, src, dest, { 0, 0 }, 0.0f, enemyTint);
        }
        else
        {
            Color color = mDoOrDieMode ? MAROON : RED;
            if (enemy->name == "Lumberjack") color = mDoOrDieMode ? DARKPURPLE : MAROON;
            if (enemy->name == "Old McDonnell") color = mDoOrDieMode ? DARKBROWN : DARKPURPLE;
            
            DrawRectangle(
                (int)(enemy->position.x - enemy->scale.x / 2.0f),
                (int)(enemy->position.y - enemy->scale.y),
                (int)enemy->scale.x,
                (int)enemy->scale.y,
                color
            );
        }
    }
}

void LevelB::renderHealthBars()
{
    for (auto* zombie : mZombies)
    {
        if (zombie->state == UNIT_DEAD || 
            zombie->state == UNIT_IDLE) continue;
        
        float healthPercent = zombie->stats.currentHealth / zombie->stats.maxHealth;
        if (healthPercent < 0) healthPercent = 0;
        
        float barX = zombie->position.x - HEALTH_BAR_WIDTH / 2.0f;
        float barY = zombie->position.y - zombie->scale.y - HEALTH_BAR_OFFSET_Y + 50;
        
        DrawRectangle((int)barX, (int)barY, 
                      (int)HEALTH_BAR_WIDTH, (int)HEALTH_BAR_HEIGHT, 
                      DARKGRAY);
        
        Color healthColor = GREEN;
        if (healthPercent < 0.5f) healthColor = YELLOW;
        if (healthPercent < 0.25f) healthColor = RED;
        
        DrawRectangle((int)barX, (int)barY, 
                      (int)(HEALTH_BAR_WIDTH * healthPercent), 
                      (int)HEALTH_BAR_HEIGHT, 
                      healthColor);
        
        DrawRectangleLines((int)barX, (int)barY, 
                           (int)HEALTH_BAR_WIDTH, (int)HEALTH_BAR_HEIGHT, 
                           BLACK);
    }
    
    for (auto* enemy : mEnemies)
    {
        if (enemy->state == UNIT_DEAD) continue;
        
        float healthPercent = enemy->stats.currentHealth / enemy->stats.maxHealth;
        if (healthPercent < 0) healthPercent = 0;
        
        float barWidth = HEALTH_BAR_WIDTH;
        if (enemy->name == "Old McDonnell") barWidth = 100.0f;
        
        float barX = enemy->position.x - barWidth / 2.0f;
        float barY = enemy->position.y - enemy->scale.y - HEALTH_BAR_OFFSET_Y;
        
        DrawRectangle((int)barX, (int)barY, 
                      (int)barWidth, (int)HEALTH_BAR_HEIGHT, 
                      DARKGRAY);
        
        DrawRectangle((int)barX, (int)barY, 
                      (int)(barWidth * healthPercent), 
                      (int)HEALTH_BAR_HEIGHT, 
                      RED);
        
        DrawRectangleLines((int)barX, (int)barY, 
                           (int)barWidth, (int)HEALTH_BAR_HEIGHT, 
                           BLACK);
    }
}

void LevelB::renderDeploymentBar()
{
    if (mCurrentDeployIndex >= (int)mZombies.size()) return;
    
    float barWidth = 100.0f;
    float barHeight = 20.0f;
    float barX = 60.0f;
    float barY = GetScreenHeight() - 200.0f;
    
    DrawRectangle((int)barX, (int)barY, (int)barWidth, (int)barHeight, DARKGRAY);
    
    Color fillColor = mCanDeploy ? GREEN : BLUE;
    DrawRectangle((int)barX, (int)barY, 
                  (int)(barWidth * mDeploymentProgress), 
                  (int)barHeight, fillColor);
    
    DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, WHITE);
    
    const char* text = mCanDeploy ? "PRESS SPACE TO DEPLOY" : "Charging...";
    DrawText(text, (int)barX, (int)(barY - 22), 18, WHITE);
}

void LevelB::renderUI()
{    
    int zombiesAlive = 0;
    int zombiesDeployed = 0;
    
    for (auto* zombie : mZombies)
    {
        if (zombie->state != UNIT_DEAD) zombiesAlive++;
        if (zombie->state == UNIT_MARCHING || zombie->state == UNIT_FIGHTING) 
            zombiesDeployed++;
    }
    
    int enemiesAlive = 0;
    for (auto* enemy : mEnemies)
    {
        if (enemy->state != UNIT_DEAD) enemiesAlive++;
    }
    
    if (!mDoOrDieMode)
    {
        int seconds = (int)mBattleTimer;
        Color timerColor = (seconds <= 5) ? RED : WHITE;
        
        DrawText(
            TextFormat("Time: %d", seconds),
            GetScreenWidth() / 2 - 40, 20, 24, timerColor
        );
    }
}

void LevelB::renderBattleResult()
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
    
    if (mPlayerWon)
    {
        DrawText("VICTORY!", 
                 GetScreenWidth() / 2 - 100, 
                 GetScreenHeight() / 2 - 50, 
                 48, GREEN);
    }
    else
    {
        DrawText("DEFEAT!", 
                 GetScreenWidth() / 2 - 80, 
                 GetScreenHeight() / 2 - 50, 
                 48, RED);
    }
}

void LevelB::shutdown()
{
    StopMusicStream(mGameState.bgm);
    if (mDoOrDieMusicLoaded)
    {
        StopMusicStream(mDoOrDieMusic);
    }
    
    for (auto* zombie : mZombies)
    {
        if (zombie->textureLoaded && zombie->texture.id != 0)
        {
            UnloadTexture(zombie->texture);
        }
        delete zombie;
    }
    mZombies.clear();
    
    for (auto* enemy : mEnemies)
    {
        delete enemy;
    }
    mEnemies.clear();
    
    unloadAssets();
}