#include "Menu.h"

Menu::Menu() 
    : Scene({ 0.0f, 0.0f }, nullptr),
      mBackgroundLoaded(false)
{
}

Menu::Menu(Vector2 origin, const char* bgHexCode)
    : Scene(origin, bgHexCode),
      mBackgroundLoaded(false)
{
}

Menu::~Menu()
{
    shutdown();
}

void Menu::initialise()
{
    mGameState.nextSceneID = -1; 
    mGameState.xochitl = nullptr;
    mGameState.map = nullptr;
    mGameState.campfire = nullptr;
    
    mBackground = LoadTexture("assets/game/menu.png");
    mBackgroundLoaded = (mBackground.id != 0);
    
    mGameState.camera = { 0 };
    mGameState.camera.target = mOrigin;
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;
}

void Menu::update(float deltaTime)
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        mGameState.nextSceneID = 1;  
    }
}

void Menu::render()
{
    if (mBackgroundLoaded)
    {
        Rectangle src = { 0, 0, (float)mBackground.width, (float)mBackground.height };
        Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
        
        DrawTexturePro(mBackground, src, dest, { 0, 0 }, 0.0f, WHITE);
    }
}

void Menu::shutdown()
{
    if (mBackgroundLoaded && mBackground.id != 0)
    {
        UnloadTexture(mBackground);
        mBackground.id = 0;
        mBackgroundLoaded = false;
    }
}
