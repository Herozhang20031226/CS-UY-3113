#include "Scene.h"

int Scene::slive = Scene::total_lives;
result Scene::sresult = result_none;

Scene::Scene() : mOrigin{{}} {}

Scene::Scene(Vector2 origin, const char *bgHexCode) : mOrigin{origin}, mBGColourHexCode {bgHexCode} 
{
    ClearBackground(ColorFromHex(bgHexCode));
}