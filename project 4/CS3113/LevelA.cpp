#include "LevelA.h"

LevelA::LevelA()                                      : Scene { {0.0f}, nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
   mGameState.levelID     = 1;
   mGameState.nextSceneID = -1;

   mGameState.bgm = LoadMusicStream("assets/game/level1.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/game/jump.mp3");
   mGameState.collide = LoadSound("assets/game/collide.mp3");
   mGameState.win = LoadSound("assets/game/win.mp3");

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/game/grass.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      1, 1,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */
   std::map<Direction, std::vector<int>> xochitlAnimationAtlas = {
      {DOWN,  {  0,  1,  2,  3,  4,  5,  6,  7 }},
      {LEFT,  {  8,  9, 10, 11, 12, 13, 14, 15 }},
      {UP,    { 24, 25, 26, 27, 28, 29, 30, 31 }},
      {RIGHT, { 40, 41, 42, 43, 44, 45, 46, 47 }},
   };

   float sizeRatio  = 48.0f / 64.0f;

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.xochitl = new Entity(
      {mOrigin.x - 300.0f, mOrigin.y-200}, // position
      {150.0f, 150.0f},             // scale
      "assets/game/flyer.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 1, 1 },                                 // atlas dimensions
      xochitlAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.xochitl->setJumpingPower(550.0f);
   mGameState.xochitl->setColliderDimensions({
      mGameState.xochitl->getScale().x / 3.5f,
      mGameState.xochitl->getScale().y/1.5f
   });
   mGameState.xochitl->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});


   //here I create the entity for wonder AI
   mGameState.enemies.clear();

   Entity *wanderer = new Entity(
      { mOrigin.x + 250.0f, mOrigin.y - 120.0f },
      {80.0f, 80.0f},
      "assets/game/flyer.png",
      ATLAS,
      { 1, 1 },
      xochitlAnimationAtlas,
      NPC
   );
   wanderer->setAIType(WANDERER);
   wanderer->setAIState(WALKING);
   wanderer->setAcceleration({0.0f,ACCELERATION_OF_GRAVITY});
   wanderer->setColliderDimensions({
      wanderer->getScale().x / 18.5f,
      wanderer->getScale().y / 1.2f
   });

   mGameState.enemies.push_back(wanderer);
   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.xochitl->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelA::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.xochitl->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      nullptr,        // collidable entities
      0               // col. entity count
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

   if (mGameState.xochitl->getPosition().x > mOrigin.x+420)
   {
      mGameState.nextSceneID = 2;   
   }


   Vector2 currentPlayerPosition = { mGameState.xochitl->getPosition().x, mOrigin.y };
   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.xochitl->render();
   mGameState.map->render();

      for (int i = 0; i < (int) mGameState.enemies.size(); i++)
   {
       mGameState.enemies[i]->render();
   }

   DrawText(TextFormat("Lives remaining: %d", Scene::get_live()), 
            (int)(mGameState.camera.target.x - 450), 
            (int)(mGameState.camera.target.y - 260), 
            20, RED);

}

void LevelA::shutdown()
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