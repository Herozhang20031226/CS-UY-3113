#include "CS3113/Menu.h"
#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 4;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

Menu *gMenuScene = nullptr;
LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelC *gLevelC = nullptr;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    gMenuScene = new Menu(ORIGIN, "#C0897E");
    gLevelA = new LevelA(ORIGIN, "#cfd8dfff");
    gLevelB    = new LevelB(ORIGIN, "#011627");
    gLevelC    = new LevelC(ORIGIN, "#cc1781ff");

    gLevels.push_back(gMenuScene);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);


    switchToScene(gLevels[0]);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose())
    {
        gAppStatus = TERMINATED;
        return;
    }
    Entity *player = gCurrentScene->getState().xochitl;
    if (player == nullptr) return;

    player->resetMovement();

    if      (IsKeyDown(KEY_A)) player->moveLeft();
    else if (IsKeyDown(KEY_D)) player->moveRight();

    if (IsKeyPressed(KEY_W) && player->isCollidingBottom())
    {
        player->jump();
        PlaySound(gCurrentScene->getState().jumpSound);
    }

    if (GetLength(player->getMovement()) > 1.0f)
        player->normaliseMovement();}

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
        deltaTime -= FIXED_TIMESTEP;
    }

    int nextID = gCurrentScene->getState().nextSceneID;
    if (nextID >= 0 && nextID < (int) gLevels.size())
    {
        switchToScene(gLevels[nextID]);
    }

}

void render()
{
    BeginDrawing();
    BeginMode2D(gCurrentScene->getState().camera);

    gCurrentScene->render();

    EndMode2D();
    EndDrawing();
}

void shutdown() 
{
    delete gMenuScene;
    delete gLevelA;
    delete gLevelB;
    delete gLevelC;
    
    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
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