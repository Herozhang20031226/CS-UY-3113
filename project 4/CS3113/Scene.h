#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

enum result{
    result_none,
    result_win,
    result_lost
};

struct GameState
{
    Entity *xochitl;
    Map *map;

    Music bgm;
    Sound jumpSound;
    Sound win;
    Sound collide;
    Camera2D camera;

    std::vector<Entity*> enemies;

    int nextSceneID = -1;
    int levelID     = -1;
};

class Scene 
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";
    
    static int slive;
    static result sresult;
public:
    //there are total of three lives
    static constexpr int total_lives = 3;

    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    static void reset()        { slive = total_lives; }
    static void lose()          { if (slive > 0) slive--; }
    static int  get_live()          { return slive; }
    static void setresult(result new_result) { sresult = new_result; }
    static result getresult() { return sresult; }

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    
    GameState   getState()           const { return mGameState; }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif