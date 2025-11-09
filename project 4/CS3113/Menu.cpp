#include "Menu.h"

Menu::Menu() : Scene{{0.0f, 0.0f}, "#000000"} {}
Menu::Menu(Vector2 origin, const char *bgHexCode) 
    : Scene{origin, bgHexCode} {}
Menu::~Menu() { shutdown(); }

void Menu::initialise()
{
    mGameState.levelID     = 0;
    mGameState.nextSceneID = -1;

    Scene::reset();

    mGameState.camera.target = { 0.0f, 0.0f };
    mGameState.camera.offset = { 0.0f, 0.0f };
    mGameState.camera.zoom   = 1.0f;
}

void Menu::update(float deltaTime)
{
    (void) deltaTime;

    if (IsKeyPressed(KEY_ENTER))
    {
        Scene::setresult(result_none);
        mGameState.nextSceneID = 1; 
    }
}

void Menu::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    DrawText("Rise Of AI", 260, 180, 40, WHITE);
    DrawText("ENTER to start",260, 260, 30, WHITE);

    if (Scene::getresult() == result_win)
        DrawText("YOU WIN!", 320, 340, 40, GREEN);
    else if (Scene::getresult() == result_lost)
        DrawText("YOU LOSE!", 320, 340, 40, RED);
}

void Menu::shutdown()
{
}
