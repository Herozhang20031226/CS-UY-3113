#include "CS3113/ShaderProgram.h"
#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/Menu.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 3; 

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

Menu   *gMenu   = nullptr;
LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;

Effects *gEffects = nullptr;

ShaderProgram gShader;
Vector2 gLightPosition = { 0.0f, 0.0f };


struct ZombieSlot
{
    const char* name;
    int cost;
    bool unlocked;
    bool upgraded;     
    int upgradeCost;    
    Texture2D icon;
    Texture2D upgradedIcon;  
    bool hasIcon;
    bool hasUpgradedIcon;   
};

constexpr int NUM_ZOMBIE_SLOTS = 5;

ZombieSlot gZombieSlots[NUM_ZOMBIE_SLOTS] = {
    { "Zombie I",   50,  false, false, 100, {}, {}, false, false },
    { "Zombie II",  100, false, false, 150, {}, {}, false, false },
    { "Zombie III", 150, false, false, 200, {}, {}, false, false },
    { "Zombie IV",  200, false, false, 250, {}, {}, false, false },
    { "Zombie V",   300, false, false, 350, {}, {}, false, false }
};

bool gOpponent1Defeated = false;  
bool gOpponent2Defeated = false;  
bool gUpgradeWindowOpen = false;
int  gSelectedUpgradeSlot = -1;

bool areAllZombiesUnlocked()
{
    for (int i = 0; i < NUM_ZOMBIE_SLOTS; i++)
    {
        if (!gZombieSlots[i].unlocked) return false;
    }
    return true;
}

bool gPurchaseWindowOpen = false;
int  gSelectedZombieSlot  = -1;
bool gLastPurchaseFailed  = false;

struct OpponentOption
{
    const char* name;

    Texture2D icon;
    bool      hasIcon;
};

Texture2D      gAttackIcon;
bool           gAttackIconLoaded   = false;
bool           gAttackWindowOpen   = false;
OpponentOption gOpponents[2];
int            gHoveredOpponent    = -1;
int            gSelectedOpponent   = -1;  

bool gInBattle = false;

bool gOnMenu = true;

void switchToScene(Scene *scene);
void switchToBattle(int opponentIndex);
void returnToFarm();
void initialise();
void processInput();
void update();
void render();
void shutdown();
void updateZombieUI();
void renderZombieUI();
void updateAttackUI();
void renderAttackUI();

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void switchToBattle(int opponentIndex)
{
    if (gLevelB != nullptr)
    {
        gLevelB->shutdown();
        delete gLevelB;
        gLevelB = nullptr;
    }
    
    gLevelB = new LevelB(ORIGIN, "#87CEEB", opponentIndex);
    
    for (int i = 0; i < NUM_ZOMBIE_SLOTS; i++)
    {
        gLevelB->setZombieUpgraded(i, gZombieSlots[i].upgraded);
    }
    
    gLevels[2] = gLevelB;  
    
    gCurrentScene = gLevelB;
    gCurrentScene->initialise();
    
    gInBattle = true;
    gAttackWindowOpen = false;
    gHoveredOpponent = -1;
}

void returnToFarm()
{    
    if (gLevelB != nullptr && gLevelB->didPlayerWin())
    {
        if (gSelectedOpponent == 0 && !gOpponent1Defeated)
        {
            gOpponent1Defeated = true;
        }
        else if (gSelectedOpponent == 1 && !gOpponent2Defeated)
        {
            gOpponent2Defeated = true;
        }
    }
    
    gInBattle = false;
    gCurrentScene = gLevelA;
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Zombie Farm Battle");
    InitAudioDevice();

    gShader.load("shaders/vertex.glsl", "shaders/fragment.glsl");

    gMenu   = new Menu(ORIGIN, "#000000");
    gLevelA = new LevelA(ORIGIN, "#C0897E");
    gLevelB = new LevelB(ORIGIN, "#87CEEB", 0);

    gLevels.push_back(gMenu);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);

    switchToScene(gLevels[0]);

    gZombieSlots[0].icon    = LoadTexture("assets/game/1.png");
    gZombieSlots[0].hasIcon = (gZombieSlots[0].icon.id != 0);

    gZombieSlots[1].icon    = LoadTexture("assets/game/2.png");
    gZombieSlots[1].hasIcon = (gZombieSlots[1].icon.id != 0);

    gZombieSlots[2].icon    = LoadTexture("assets/game/3.png");
    gZombieSlots[2].hasIcon = (gZombieSlots[2].icon.id != 0);

    gZombieSlots[3].icon    = LoadTexture("assets/game/4.png");
    gZombieSlots[3].hasIcon = (gZombieSlots[3].icon.id != 0);

    gZombieSlots[4].icon    = LoadTexture("assets/game/5.png");
    gZombieSlots[4].hasIcon = (gZombieSlots[4].icon.id != 0);

    gZombieSlots[0].upgradedIcon    = LoadTexture("assets/game/6.png");
    gZombieSlots[0].hasUpgradedIcon = (gZombieSlots[0].upgradedIcon.id != 0);

    gZombieSlots[1].upgradedIcon    = LoadTexture("assets/game/7.png");
    gZombieSlots[1].hasUpgradedIcon = (gZombieSlots[1].upgradedIcon.id != 0);

    gZombieSlots[2].upgradedIcon    = LoadTexture("assets/game/8.png");
    gZombieSlots[2].hasUpgradedIcon = (gZombieSlots[2].upgradedIcon.id != 0);

    gZombieSlots[3].upgradedIcon    = LoadTexture("assets/game/9.png");
    gZombieSlots[3].hasUpgradedIcon = (gZombieSlots[3].upgradedIcon.id != 0);

    gZombieSlots[4].upgradedIcon    = LoadTexture("assets/game/10.png");
    gZombieSlots[4].hasUpgradedIcon = (gZombieSlots[4].upgradedIcon.id != 0);

    gAttackIcon       = LoadTexture("assets/game/attack.png");
    gAttackIconLoaded = (gAttackIcon.id != 0);

    gOpponents[0].name        = "Old McDonnell's Farm";
    gOpponents[0].icon        = LoadTexture("assets/game/enemy1.png");
    gOpponents[0].hasIcon     = (gOpponents[0].icon.id != 0);

    gOpponents[1].name        = "Ninjas";
    gOpponents[1].icon        = LoadTexture("assets/game/enemy2.png");
    gOpponents[1].hasIcon     = (gOpponents[1].icon.id != 0);

    gEffects = new Effects(ORIGIN, (float) SCREEN_WIDTH * 1.5f, (float) SCREEN_HEIGHT * 1.5f);

    gEffects->setEffectSpeed(0.5f);
    gEffects->start(FADEIN);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (gCurrentScene->getState().xochitl != nullptr){
        gCurrentScene->getState().xochitl->resetMovement();
        if (IsKeyDown(KEY_A)) gCurrentScene->getState().xochitl->moveLeft();
        if (IsKeyDown(KEY_D)) gCurrentScene->getState().xochitl->moveRight();
        if (IsKeyDown(KEY_W)) gCurrentScene->getState().xochitl->moveUp();
        if (IsKeyDown(KEY_S)) gCurrentScene->getState().xochitl->moveDown();

        if (GetLength(gCurrentScene->getState().xochitl->getMovement()) > 1.0f) 
            gCurrentScene->getState().xochitl->normaliseMovement();
        }
        if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
    }

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);

        if (gOnMenu)
        {
            if (gCurrentScene->getState().nextSceneID == 1)
            {
                gOnMenu = false;
                switchToScene(gLevels[1]); 
            }
        }
        else if (!gInBattle)
        {
            if (gCurrentScene->getState().xochitl != nullptr &&
                gCurrentScene->getState().campfire != nullptr)
            {
                Vector2 cameraTarget = gCurrentScene->getState().xochitl->getPosition();
                gEffects->update(FIXED_TIMESTEP, &cameraTarget);
                gLightPosition = gCurrentScene->getState().campfire->getPosition();
            }

            updateZombieUI();
            updateAttackUI();
        }
        else
        {
            if (gCurrentScene->getState().nextSceneID == 0)
            {
                returnToFarm();
            }
        }

        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    
    if (gOnMenu)
    {
        gCurrentScene->render();
    }
    else if (!gInBattle)
    {
        BeginMode2D(gCurrentScene->getState().camera);
        gShader.begin();

        gShader.setVector2("lightPosition", gLightPosition);
        gCurrentScene->render();

        gShader.end();
        gEffects->render();
        EndMode2D();

        if (gCurrentScene->getState().goldIcon.id != 0)
        {
            Texture2D goldIcon = gCurrentScene->getState().goldIcon;
            int goldAmount     = gCurrentScene->getState().gold;

            int iconX = 20;
            int iconY = 20;

            DrawTexture(goldIcon, iconX, iconY, WHITE);

            DrawText(
                TextFormat("%d", goldAmount),
                iconX + goldIcon.width + 10,
                iconY + 4,
                24,
                WHITE
            );
        }
        
        renderAttackUI();
        renderZombieUI();
    }
    else
    {
        gCurrentScene->render();
    }

    EndDrawing();
}

void shutdown() 
{
    for (int i = 0; i < NUM_ZOMBIE_SLOTS; i++)
    {
        if (gZombieSlots[i].hasIcon && gZombieSlots[i].icon.id != 0)
        {
            UnloadTexture(gZombieSlots[i].icon);
            gZombieSlots[i].icon.id = 0;
            gZombieSlots[i].hasIcon = false;
        }
        
        if (gZombieSlots[i].hasUpgradedIcon && gZombieSlots[i].upgradedIcon.id != 0)
        {
            UnloadTexture(gZombieSlots[i].upgradedIcon);
            gZombieSlots[i].upgradedIcon.id = 0;
            gZombieSlots[i].hasUpgradedIcon = false;
        }
    }

    if (gAttackIconLoaded && gAttackIcon.id != 0)
    {
        UnloadTexture(gAttackIcon);
        gAttackIcon.id     = 0;
        gAttackIconLoaded  = false;
    }

    for (int i = 0; i < 2; i++)
    {
        if (gOpponents[i].hasIcon && gOpponents[i].icon.id != 0)
        {
            UnloadTexture(gOpponents[i].icon);
            gOpponents[i].icon.id   = 0;
            gOpponents[i].hasIcon   = false;
        }
    }

    if (gMenu != nullptr)
    {
        delete gMenu;
        gMenu = nullptr;
    }
    
    if (gLevelA != nullptr)
    {
        delete gLevelA;
        gLevelA = nullptr;
    }
    
    if (gLevelB != nullptr)
    {
        delete gLevelB;
        gLevelB = nullptr;
    }

    gLevels.clear();

    if (gEffects != nullptr)
    {
        delete gEffects;
        gEffects = nullptr;
    }

    gShader.unload();

    CloseAudioDevice();
    CloseWindow();
}

static void computeBarLayout(float& barX, float& barY,
                             float& barWidth, float& barHeight,
                             float& slotWidth, float& slotHeight,
                             float& slotPadding)
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float barMargin = 20.0f;
    barWidth  = (float)screenW - barMargin * 2.0f;
    barHeight = 90.0f;
    barX      = barMargin;
    barY      = (float)screenH - barHeight - barMargin;

    slotPadding = 10.0f;
    slotWidth   = (barWidth - slotPadding * (NUM_ZOMBIE_SLOTS + 1)) / NUM_ZOMBIE_SLOTS;
    slotHeight  = barHeight - 2.0f * slotPadding;
}

void updateZombieUI()
{
    LevelA* levelA = dynamic_cast<LevelA*>(gCurrentScene);
    if (!levelA) return;

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;

    Vector2 mouse = GetMousePosition();
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    if (gPurchaseWindowOpen && 
        gSelectedZombieSlot >= 0 && gSelectedZombieSlot < NUM_ZOMBIE_SLOTS)
    {
        float windowWidth  = 320.0f;
        float windowHeight = 220.0f;
        float windowX = (screenW - windowWidth)  / 2.0f;
        float windowY = (screenH - windowHeight) / 2.0f;

        float buttonWidth  = 110.0f;
        float buttonHeight = 40.0f;
        float padding      = 20.0f;

        Rectangle buyRect = {
            windowX + padding,
            windowY + windowHeight - buttonHeight - padding,
            buttonWidth,
            buttonHeight
        };
        Rectangle cancelRect = {
            windowX + windowWidth - buttonWidth - padding,
            windowY + windowHeight - buttonHeight - padding,
            buttonWidth,
            buttonHeight
        };
        Rectangle windowRect = { windowX, windowY, windowWidth, windowHeight };

        if (CheckCollisionPointRec(mouse, cancelRect))
        {
            gPurchaseWindowOpen = false;
            gSelectedZombieSlot = -1;
            gLastPurchaseFailed = false;
            return;
        }

        if (CheckCollisionPointRec(mouse, buyRect))
        {
            ZombieSlot& slot = gZombieSlots[gSelectedZombieSlot];
            
            bool canUpgrade = slot.unlocked && !slot.upgraded && gOpponent1Defeated;
            
            if (canUpgrade)
            {
                if (levelA->spendGold(slot.upgradeCost))
                {
                    slot.upgraded = true;
                    gPurchaseWindowOpen = false;
                    gLastPurchaseFailed = false;
                    printf("Upgraded %s!\n", slot.name);
                }
                else
                {
                    gLastPurchaseFailed = true;
                }
            }
            else if (!slot.unlocked)
            {
                if (levelA->spendGold(slot.cost))
                {
                    slot.unlocked = true;
                    gPurchaseWindowOpen = false;
                    gLastPurchaseFailed = false;
                }
                else
                {
                    gLastPurchaseFailed = true;
                }
            }
            else
            {
                gPurchaseWindowOpen = false;
                gLastPurchaseFailed = false;
            }

            return;
        }

        if (!CheckCollisionPointRec(mouse, windowRect))
        {
            gPurchaseWindowOpen = false;
            gSelectedZombieSlot = -1;
            gLastPurchaseFailed = false;
        }

        return;
    }

    float barX, barY, barW, barH, slotW, slotH, slotPad;
    computeBarLayout(barX, barY, barW, barH, slotW, slotH, slotPad);

    for (int i = 0; i < NUM_ZOMBIE_SLOTS; i++)
    {
        float x = barX + slotPad + i * (slotW + slotPad);
        float y = barY + slotPad;
        Rectangle slotRect = { x, y, slotW, slotH };

        if (CheckCollisionPointRec(mouse, slotRect))
        {
            gSelectedZombieSlot = i;
            gPurchaseWindowOpen = true;
            gLastPurchaseFailed = false;
            return;
        }
    }
}

void updateAttackUI()
{
    if (gInBattle) return;
    
    LevelA* levelA = dynamic_cast<LevelA*>(gCurrentScene);
    if (!levelA) return;

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;

    if (gPurchaseWindowOpen) return;

    Vector2 mouse   = GetMousePosition();
    int     screenW = GetScreenWidth();
    int     screenH = GetScreenHeight();

    float iconSize = 40.0f;
    float iconX    = 20.0f;
    float iconY    = 70.0f;
    Rectangle attackIconRect = { iconX, iconY, iconSize + 50, iconSize };

    if (gAttackWindowOpen)
    {
        float windowWidth  = 600.0f;
        float windowHeight = 260.0f;
        float windowX = (screenW - windowWidth)  / 2.0f;
        float windowY = (screenH - windowHeight) / 2.0f;

        Rectangle windowRect = { windowX, windowY, windowWidth, windowHeight };

        float closeW = 90.0f;
        float closeH = 38.0f;
        Rectangle closeBtn = {
            windowX + windowWidth - closeW - 16.0f,
            windowY + windowHeight - closeH - 16.0f,
            closeW,
            closeH
        };

        float cardGap    = 24.0f;
        float cardWidth  = (windowWidth - 3 * cardGap) / 2.0f;
        float cardHeight = windowHeight - 90.0f;
        float cardY      = windowY + 60.0f;

        Rectangle cards[2] = {
            { windowX + cardGap,                    cardY, cardWidth, cardHeight },
            { windowX + cardGap * 2.0f + cardWidth, cardY, cardWidth, cardHeight }
        };

        if (CheckCollisionPointRec(mouse, closeBtn))
        {
            gAttackWindowOpen = false;
            gHoveredOpponent  = -1;
            return;
        }

        for (int i = 0; i < 2; i++)
        {
            if (CheckCollisionPointRec(mouse, cards[i]))
            {
                gSelectedOpponent = i;        
                switchToBattle(i);
                return;
            }
        }

        if (!CheckCollisionPointRec(mouse, windowRect))
        {
            gAttackWindowOpen = false;
            gHoveredOpponent  = -1;
        }

        return;
    }

    if (CheckCollisionPointRec(mouse, attackIconRect))
    {
        if (areAllZombiesUnlocked())
        {
            gAttackWindowOpen = true;
            gHoveredOpponent  = -1;
        }
    }
}

void renderZombieUI()
{
    if (gInBattle) return;
    
    LevelA* levelA = dynamic_cast<LevelA*>(gCurrentScene);
    if (!levelA) return;

    float barX, barY, barW, barH, slotW, slotH, slotPad;
    computeBarLayout(barX, barY, barW, barH, slotW, slotH, slotPad);

    DrawRectangleRounded(
        { barX, barY, barW, barH },
        0.2f,
        8,
        Fade(BLACK, 0.5f)
    );

    bool allUnlocked = true;

    for (int i = 0; i < NUM_ZOMBIE_SLOTS; i++)
    {
        const ZombieSlot& slot = gZombieSlots[i];

        float x = barX + slotPad + i * (slotW + slotPad);
        float y = barY + slotPad;
        Rectangle slotRect = { x, y, slotW, slotH };

        Color slotColor = DARKGRAY;
        if (slot.unlocked) slotColor = DARKGREEN;
        if (slot.upgraded) slotColor = DARKBLUE;  
        
        DrawRectangleRounded(slotRect, 0.25f, 6, Fade(slotColor, 0.9f));
        DrawRectangleRoundedLines(slotRect, 0.25f, 6, RAYWHITE);

        DrawText(
            slot.name,
            (int)(x + 8),
            (int)(y + 8),
            18,
            WHITE
        );

        if (slot.unlocked && !slot.upgraded && gOpponent1Defeated)
        {
            DrawText(
                TextFormat("Upgrade: %d", slot.upgradeCost),
                (int)(x + 8),
                (int)(y + 34),
                14,
                SKYBLUE
            );
        }

        const char* status;
        Color statusColor;
        
        if (slot.upgraded)
        {
            status = "UPGRADED";
            statusColor = SKYBLUE;
        }
        else if (slot.unlocked)
        {
            status = "Unlocked";
            statusColor = LIME;
        }
        else
        {
            status = "Locked";
            statusColor = LIGHTGRAY;
        }

        int textWidth = MeasureText(status, 14);
        DrawText(
            status,
            (int)(x + slotW - textWidth - 8),
            (int)(y + slotH - 24),
            14,
            statusColor
        );

        if (!slot.unlocked) allUnlocked = false;
        
        Texture2D* iconToDraw = nullptr;
        
        if (slot.upgraded && slot.hasUpgradedIcon && slot.upgradedIcon.id != 0)
        {
            iconToDraw = (Texture2D*)&slot.upgradedIcon;
        }
        else if (slot.hasIcon && slot.icon.id != 0)
        {
            iconToDraw = (Texture2D*)&slot.icon;
        }
        
        if (iconToDraw != nullptr)
        {
            float iconMargin = 8.0f;
            float iconSz     = slotH - iconMargin * 2.0f;
    
            Rectangle src = {
                0.0f,
                0.0f,
                (float)iconToDraw->width,
                (float)iconToDraw->height
            };
    
            Rectangle dest = {
                x + slotW - iconSz - iconMargin,
                y + iconMargin - 20,
                iconSz,
                iconSz
            };
    
            DrawTexturePro(
                *iconToDraw,
                src,
                dest,
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        }    
    }

    if (gPurchaseWindowOpen &&
        gSelectedZombieSlot >= 0 && gSelectedZombieSlot < NUM_ZOMBIE_SLOTS)
    {
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        float windowWidth  = 320.0f;
        float windowHeight = 220.0f;
        float windowX = (screenW - windowWidth)  / 2.0f;
        float windowY = (screenH - windowHeight) / 2.0f;

        Rectangle windowRect = { windowX, windowY, windowWidth, windowHeight };

        DrawRectangleRounded(windowRect, 0.25f, 8, Fade(BLACK, 0.85f));
        DrawRectangleRoundedLines(windowRect, 0.25f, 8, RAYWHITE);

        const ZombieSlot& slot = gZombieSlots[gSelectedZombieSlot];

        bool canUpgrade = slot.unlocked && !slot.upgraded && gOpponent1Defeated;
        
        const char* title = canUpgrade ? "Upgrade Zombie" : "Recruit Zombie";
        int titleWidth = MeasureText(title, 22);
        DrawText(
            title,
            (int)(windowX + (windowWidth - titleWidth) / 2.0f),
            (int)(windowY + 12),
            22,
            canUpgrade ? SKYBLUE : WHITE
        );

        DrawText(
            TextFormat("Unit: %s", slot.name),
            (int)(windowX + 20),
            (int)(windowY + 50),
            18,
            WHITE
        );
        
        if (canUpgrade)
        {
            DrawText(
                TextFormat("Upgrade Cost: %d gold", slot.upgradeCost),
                (int)(windowX + 20),
                (int)(windowY + 80),
                18,
                SKYBLUE
            );
            
        }
        else if (!slot.unlocked)
        {
            DrawText(
                TextFormat("Cost: %d gold", slot.cost),
                (int)(windowX + 20),
                (int)(windowY + 80),
                18,
                GOLD
            );
        }
        else
        {
            DrawText(
                "Already unlocked!",
                (int)(windowX + 20),
                (int)(windowY + 80),
                18,
                LIME
            );
            
            if (!gOpponent1Defeated)
            {
                DrawText(
                    "Defeat Opponent 1 to unlock upgrades",
                    (int)(windowX + 20),
                    (int)(windowY + 105),
                    12,
                    YELLOW
                );
            }
        }

        int currentGold = gCurrentScene->getState().gold;


        if (gLastPurchaseFailed)
        {
            const char* warn = "Not enough gold!";
            int warnW = MeasureText(warn, 16);
            DrawText(
                warn,
                (int)(windowX + (windowWidth - warnW) / 2.0f),
                (int)(windowY + 155),
                16,
                RED
            );
        }

        float buttonWidth  = 110.0f;
        float buttonHeight = 40.0f;
        float padding      = 20.0f;

        Rectangle buyRect = {
            windowX + padding,
            windowY + windowHeight - buttonHeight - padding,
            buttonWidth,
            buttonHeight
        };
        Rectangle cancelRect = {
            windowX + windowWidth - buttonWidth - padding,
            windowY + windowHeight - buttonHeight - padding,
            buttonWidth,
            buttonHeight
        };

        int requiredGold = canUpgrade ? slot.upgradeCost : slot.cost;
        bool canAfford = currentGold >= requiredGold;
        bool showButton = !slot.unlocked || canUpgrade;
        
        Color buyColor = (canAfford && showButton) ? DARKGREEN : DARKGRAY;
        if (canUpgrade && canAfford) buyColor = DARKBLUE;
        
        DrawRectangleRounded(buyRect, 0.3f, 6, buyColor);
        DrawRectangleRoundedLines(buyRect, 0.3f, 6, RAYWHITE);
        
        const char* buttonText = canUpgrade ? "UPGRADE" : "BUY";
        DrawText(
            buttonText,
            (int)(buyRect.x + (canUpgrade ? 15 : 30)),
            (int)(buyRect.y + 10),
            20,
            WHITE
        );

        DrawRectangleRounded(cancelRect, 0.3f, 6, DARKGRAY);
        DrawRectangleRoundedLines(cancelRect, 0.3f, 6, RAYWHITE);
        DrawText(
            "CANCEL",
            (int)(cancelRect.x + 12),
            (int)(cancelRect.y + 10),
            20,
            WHITE
        );
    }
}

void renderAttackUI()
{
    if (gInBattle) return;
    
    LevelA* levelA = dynamic_cast<LevelA*>(gCurrentScene);
    if (!levelA) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float iconSize = 40.0f;
    float iconX    = 20.0f;
    float iconY    = 70.0f;
    Rectangle attackIconRect = { iconX, iconY, iconSize + 50, iconSize };

    if (gAttackIconLoaded && gAttackIcon.id != 0)
    {
        Rectangle src  = { 0.0f, 0.0f,
                           (float)gAttackIcon.width,
                           (float)gAttackIcon.height };
        Rectangle dest = attackIconRect;

        DrawTexturePro(
            gAttackIcon,
            src,
            dest,
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }
    else
    {
        DrawRectangleRec(attackIconRect, RED);
        DrawText("!", (int)(iconX + 12), (int)(iconY + 8), 24, WHITE);
    }

    if (!gAttackWindowOpen) return;

    float windowWidth  = 600.0f;
    float windowHeight = 260.0f;
    float windowX = (screenW - windowWidth)  / 2.0f;
    float windowY = (screenH - windowHeight) / 2.0f;

    Rectangle windowRect = { windowX, windowY, windowWidth, windowHeight };

    DrawRectangleRounded(windowRect, 0.25f, 8, Fade(BLACK, 0.9f));
    DrawRectangleRoundedLines(windowRect, 0.25f, 8, RAYWHITE);

    const char* title = "Choose an opponent";
    int titleWidth    = MeasureText(title, 24);
    DrawText(
        title,
        (int)(windowX + (windowWidth - titleWidth) / 2.0f),
        (int)(windowY + 12.0f),
        24,
        WHITE
    );

    float cardGap    = 24.0f;
    float cardWidth  = (windowWidth - 3 * cardGap) / 2.0f;
    float cardHeight = windowHeight - 90.0f;
    float cardY      = windowY + 60.0f;

    for (int i = 0; i < 2; i++)
    {
        float cardX = windowX + cardGap + i * (cardWidth + cardGap);
        Rectangle cardRect = { cardX, cardY, cardWidth, cardHeight };

        DrawRectangleRounded(cardRect, 0.2f, 6, Fade(DARKGRAY, 0.85f));
        DrawRectangleRoundedLines(cardRect, 0.2f, 6, RAYWHITE);

        if (gOpponents[i].hasIcon && gOpponents[i].icon.id != 0)
        {
            float maxIconW = cardWidth  - 2.0f * 20.0f;
            float maxIconH = cardHeight * 0.55f;

            float texW = (float)gOpponents[i].icon.width;
            float texH = (float)gOpponents[i].icon.height;
            float aspect = texW / texH;

            float iconW = maxIconW;
            float iconH = iconW / aspect;

            if (iconH > maxIconH)
            {
                iconH = maxIconH;
                iconW = iconH * aspect;
            }

            float iconX2 = cardX + (cardWidth  - iconW) / 2.0f;
            float iconY2 = cardY + 10.0f;

            Rectangle src = { 0.0f, 0.0f, texW, texH };
            Rectangle dest = { iconX2, iconY2, iconW, iconH };

            DrawTexturePro(
                gOpponents[i].icon,
                src,
                dest,
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        }

        DrawText(
            gOpponents[i].name,
            (int)(cardX + 12.0f),
            (int)(cardY + cardHeight * 0.60f),
            18,
            YELLOW
        );

        DrawText(
            "Click to Attack!",
            (int)(cardX + 12.0f),
            (int)(cardY + cardHeight - 30.0f),
            14,
            LIGHTGRAY
        );
    }

    float closeW = 90.0f;
    float closeH = 38.0f;
    float closeBtnX = windowX + windowWidth - closeW - 16.0f;
    float closeBtnY = windowY + windowHeight - closeH - 16.0f;
    
    DrawRectangleRounded({ closeBtnX, closeBtnY, closeW, closeH }, 0.3f, 6, DARKGRAY);
    DrawRectangleRoundedLines({ closeBtnX, closeBtnY, closeW, closeH }, 0.3f, 6, RAYWHITE);
    DrawText("CLOSE", (int)(closeBtnX + 15), (int)(closeBtnY + 10), 18, WHITE);
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}