#include "LevelC.h"

LevelC::LevelC()                                      : Scene { {0.0f, 0.0f}, nullptr   } {}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene { origin,      bgHexCode } {}
LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{
    mGameState.levelID     = 3;
    mGameState.nextSceneID = -1;

    mGameState.bgm = LoadMusicStream("assets/game/level3.mp3");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/jump.mp3");
    mGameState.collide  = LoadSound("assets/game/collide.mp3");
    mGameState.win  = LoadSound("assets/game/win.mp3");

    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,
        (unsigned int *) mLevelData,
        "assets/game/grass.png",
        TILE_DIMENSION,
        1, 1,
        mOrigin
    );

    std::map<Direction, std::vector<int>> xochitlAnimationAtlas = {
       {DOWN,  {  0,  1,  2,  3,  4,  5,  6,  7 }},
       {LEFT,  {  8,  9, 10, 11, 12, 13, 14, 15 }},
       {UP,    { 24, 25, 26, 27, 28, 29, 30, 31 }},
       {RIGHT, { 40, 41, 42, 43, 44, 45, 46, 47 }},
    };

    float sizeRatio  = 48.0f / 64.0f;

    mGameState.xochitl = new Entity(
       {mOrigin.x - 300.0f, mOrigin.y - 200.0f},
       {150.0f, 150.0f},
       "assets/game/flyer.png",
       ATLAS,
       { 1, 1 },
       xochitlAnimationAtlas,
       PLAYER
    );

    mGameState.xochitl->setJumpingPower(550.0f);
    mGameState.xochitl->setColliderDimensions({
       mGameState.xochitl->getScale().x / 3.5f,
       mGameState.xochitl->getScale().y / 1.2f
    });
    mGameState.xochitl->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

    // ------ AI: FLYER ------
    mGameState.enemies.clear();

    Entity *flyer = new Entity(
        { mOrigin.x + 300.0f, mOrigin.y - 150.0f },
        {80.0f, 80.0f},
        "assets/game/flyer.png",
        ATLAS,
        { 1, 1 },
        xochitlAnimationAtlas,
        NPC
    );
    flyer->setAIType(FLYER);
    flyer->setAIState(WALKING);
    flyer->setAcceleration({0.0f, 0.0f}); 

    mGameState.enemies.push_back(flyer);

    mGameState.camera = { 0 };
    mGameState.camera.target   = mGameState.xochitl->getPosition();
    mGameState.camera.offset   = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom     = 1.0f;
}

void LevelC::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    mGameState.xochitl->update(
       deltaTime,
       nullptr,
       mGameState.map,
       nullptr,
       0
    );

    for (int i = 0; i < (int) mGameState.enemies.size(); i++)
    {
        Entity *enemy = mGameState.enemies[i];
        enemy->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);

        if (mGameState.xochitl->collidesWith(enemy))
        {
            PlaySound(mGameState.collide);
            Scene::lose();

            if (Scene::get_live() <= 0)
            {
                Scene::setresult(result_lost);
                mGameState.nextSceneID = 0;
            }
            else
            {
                mGameState.nextSceneID = mGameState.levelID; 
            }
            return;
        }
    }

    if (mGameState.xochitl->getPosition().y > END_GAME_THRESHOLD)
    {
        PlaySound(mGameState.collide);
        Scene::lose();

        if (Scene::get_live() <= 0)
        {
            Scene::setresult(result_lost);
            mGameState.nextSceneID = 0;
        }
        else
        {
            mGameState.nextSceneID = mGameState.levelID;
        }
        return;
    }

    if (mGameState.xochitl->getPosition().x > 
        mGameState.map->getRightBoundary() - TILE_DIMENSION-30)
    {
        Scene::setresult(result_win);
        PlaySound(mGameState.win);
        mGameState.nextSceneID = 0;
        return;
    }

    Vector2 currentPlayerPosition = { 
        mGameState.xochitl->getPosition().x, 
        mOrigin.y 
    };
    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelC::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.xochitl->render();

    for (int i = 0; i < (int) mGameState.enemies.size(); i++)
    {
        mGameState.enemies[i]->render();
    }

    DrawText(TextFormat("Lives: %d", Scene::get_live()), 
             (int)(mGameState.camera.target.x - 450), 
             (int)(mGameState.camera.target.y - 260), 
             20, RAYWHITE);
}

void LevelC::shutdown()
{
    delete mGameState.xochitl;
    delete mGameState.map;

    for (int i = 0; i < (int) mGameState.enemies.size(); i++)
    {
        delete mGameState.enemies[i];
    }
    mGameState.enemies.clear();

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.collide);
    UnloadSound(mGameState.win);
}
