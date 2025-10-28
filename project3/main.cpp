/**
* Author: [Hero Zhang]
* Assignment: Pong Clone
* Date due: 2025-10-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;


constexpr char BG_COLOUR[]    = "#C0897E";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };


constexpr int   NUMBER_OF_TILES         = 10;
constexpr float TILE_DIMENSION          = 50.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f;
constexpr float GRAVITY                 = 200.0f;
constexpr float fuel_x                = 400.0f;
constexpr float fuel_y                = 500.0f;
constexpr float air_resistent            = 1.8f;

constexpr float fuel = 100.0f;
constexpr float fuel_consumption = 5.0f;

constexpr int win_tile_index = 6;
enum class GameState { PLAY, WIN, LOSE };

// Global Variables
AppStatus gAppStatus = RUNNING;
GameState gGameState = GameState::PLAY;
float gPreviousTicks = 0.0f;
float gTimeAccumulator = 0.0f;

Entity* gSpaceship = nullptr;
Entity* gTiles = nullptr;
Entity  gTileWin;

Entity gPlatform;
float  gMovingDir = 1.0f;
constexpr float MOVING_SPEED = 140.0f;
constexpr float MOVING_LEFT  = 160.0f;
constexpr float MOVING_RIGHT = SCREEN_WIDTH - 160.0f;

Vector2 gVelocity   = { 0.0f, 0.0f };
Vector2 gAccelInput = { 0.0f, GRAVITY };
float gfuel = fuel;

Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols);

static bool AABBOverlaps(const Entity &a, const Entity &b)
{
    Vector2 ap = a.getPosition();
    Vector2 asz = a.getColliderDimensions();
    Vector2 bp = b.getPosition();
    Vector2 bsz = b.getColliderDimensions();

    float axL = ap.x - asz.x * 0.5f, axR = ap.x + asz.x * 0.5f;
    float ayT = ap.y - asz.y * 0.5f, ayB = ap.y + asz.y * 0.5f;
    float bxL = bp.x - bsz.x * 0.5f, bxR = bp.x + bsz.x * 0.5f;
    float byT = bp.y - bsz.y * 0.5f, byB = bp.y + bsz.y * 0.5f;

    return (axL < bxR && axR > bxL && ayT < byB && ayB > byT);
}

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "project3");
    SetTargetFPS(FPS);

    gSpaceship = new Entity({ ORIGIN.x, 120.0f }, { 30.0f, 30.0f }, "assets/game/rocket.png");
    gSpaceship->setColliderDimensions({ 30.0f, 30.0f }); 
    gSpaceship->setAcceleration({ 0.0f, GRAVITY });

    gTiles = new Entity[NUMBER_OF_TILES];
    float leftMostX = ORIGIN.x - (NUMBER_OF_TILES * TILE_DIMENSION) / 2.0f + TILE_DIMENSION * 0.5f;
    float centerY   = SCREEN_HEIGHT - TILE_DIMENSION * 0.5f;

    for (int i = 0; i < NUMBER_OF_TILES; ++i)
    {
        if (i == win_tile_index) continue;
        gTiles[i].setTexture("assets/game/tile.png");
        gTiles[i].setScale({ TILE_DIMENSION, TILE_DIMENSION });
        gTiles[i].setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION });
        gTiles[i].setPosition({ leftMostX + i * TILE_DIMENSION, centerY });
    }

    const float WIN_LIFT = TILE_DIMENSION * 1.3f;
    gTileWin.setTexture("assets/game/tilewin.png");
    gTileWin.setScale({ TILE_DIMENSION, TILE_DIMENSION });
    gTileWin.setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION});
    gTileWin.setPosition({ leftMostX + win_tile_index * TILE_DIMENSION, centerY - WIN_LIFT });

    float movingY = centerY - TILE_DIMENSION * 2.0f;
    gPlatform.setTexture("assets/game/tile.png");
    gPlatform.setScale({ TILE_DIMENSION, TILE_DIMENSION});
    gPlatform.setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION});
    gPlatform.setPosition({ ORIGIN.x, movingY });
}


void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) { gAppStatus = TERMINATED; }
    if (gGameState != GameState::PLAY) return;

    Vector2 accel = { 0.0f, GRAVITY };
    bool fuel_use = false;

    if (IsKeyDown(KEY_A) && gfuel > 0.0f) { accel.x -= fuel_x; fuel_use = true; }
    if (IsKeyDown(KEY_D) && gfuel > 0.0f) { accel.x += fuel_x; fuel_use = true; }
    if (IsKeyDown(KEY_W) && gfuel > 0.0f) { accel.y -= fuel_y; fuel_use = true; }

    if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_W)) && gfuel > 0.0f)
        gfuel = std::max(0.0f, gfuel - fuel_consumption);

    if (gfuel <= 0.0f) { accel.x = 0.0f; accel.y = GRAVITY; fuel_use = false; }

    gSpaceship->setAcceleration(accel);
    gSpaceship->setAngle(fuel_use ? (IsKeyDown(KEY_A) ? -6.0f : (IsKeyDown(KEY_D) ? 6.0f : 0.0f)) : 0.0f);
    gAccelInput = accel;
}


void update()
{
    float ticks = (float)GetTime();
    float dt    = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    gTimeAccumulator += dt;
    if (gTimeAccumulator < FIXED_TIMESTEP) return;

    while (gTimeAccumulator >= FIXED_TIMESTEP)
    {
        if (gGameState != GameState::PLAY)
        {
            gAccelInput = { 0.0f, 0.0f };
            gVelocity   = { 0.0f, 0.0f };
            gTimeAccumulator -= FIXED_TIMESTEP;
            continue;
        }

        Vector2 mp = gPlatform.getPosition();
        mp.x += gMovingDir * MOVING_SPEED * FIXED_TIMESTEP;
        if (mp.x < MOVING_LEFT)  { mp.x = MOVING_LEFT;  gMovingDir = 1.0f; }
        if (mp.x > MOVING_RIGHT) { mp.x = MOVING_RIGHT; gMovingDir = -1.0f; }
        gPlatform.setPosition(mp);

        gVelocity.x += gAccelInput.x * FIXED_TIMESTEP;
        gVelocity.x -= gVelocity.x * air_resistent  * FIXED_TIMESTEP;
        gVelocity.y += gAccelInput.y * FIXED_TIMESTEP;

        Vector2 p = gSpaceship->getPosition();
        p.x += gVelocity.x * FIXED_TIMESTEP;
        p.y += gVelocity.y * FIXED_TIMESTEP;
        gSpaceship->setPosition(p);

        auto half = [](const Vector2& v){ return Vector2{ v.x * 0.5f, v.y * 0.5f }; };
        auto snap_on_top = [&](const Entity& tile){
            Vector2 shipHalf = half(gSpaceship->getColliderDimensions());
            Vector2 tileHalf = half(tile.getColliderDimensions());
            Vector2 pos = gSpaceship->getPosition();
            pos.y = tile.getPosition().y - tileHalf.y - shipHalf.y;
            gSpaceship->setPosition(pos);
            gVelocity.y = 0.0f;
            gSpaceship->setAngle(0.0f);
        };

        if (AABBOverlaps(*gSpaceship, gTileWin))
        {
            snap_on_top(gTileWin);
            Vector2 pos = gSpaceship->getPosition();
            pos.x = gTileWin.getPosition().x;
            gSpaceship->setPosition(pos);

            gGameState = GameState::WIN;
            gAccelInput = { 0.0f, 0.0f };
            gVelocity   = { 0.0f, 0.0f };
            gTimeAccumulator -= FIXED_TIMESTEP;
            continue;
        }

        if (AABBOverlaps(*gSpaceship, gPlatform))
        {
            gGameState = GameState::LOSE;
            gAccelInput = { 0.0f, 0.0f };
            gVelocity   = { 0.0f, 0.0f };
        }
        else
        {
            bool hitDanger = false;
            for (int i = 0; i < NUMBER_OF_TILES; ++i)
            {
                if (i == win_tile_index) continue;
                if (!AABBOverlaps(*gSpaceship, gTiles[i])) continue;

                if (gVelocity.y >= 0.0f && gSpaceship->getPosition().y <= gTiles[i].getPosition().y)
                    snap_on_top(gTiles[i]);

                hitDanger = true;
                break;
            }

            if (hitDanger)
            {
                gGameState = GameState::LOSE;
                gAccelInput = { 0.0f, 0.0f };
                gVelocity   = { 0.0f, 0.0f };
            }
        }

        gTimeAccumulator -= FIXED_TIMESTEP;
    }
}

void DrawFuelUI()
{
    const float pad = 12.0f;
    const float w   = 240.0f;
    const float h   = 18.0f;

    DrawText("FUEL", (int)pad, (int)pad, 20, RAYWHITE);

    Rectangle box = { pad, pad + 24.0f, w, h };
    DrawRectangleLinesEx(box, 2.0f, RAYWHITE);

    float pct = gfuel / fuel;               
    if (pct < 0.0f) pct = 0.0f;
    if (pct > 1.0f) pct = 1.0f;

    // Color shifts as fuel gets low
    Color fill = GREEN;
    if (pct < 0.33f) fill = ORANGE;
    if (pct < 0.10f) fill = RED;

    int fx = (int)(box.x + 2);
    int fy = (int)(box.y + 2);
    int fw = (int)((w - 4) * pct);
    int fh = (int)(h - 4);
    DrawRectangle(fx, fy, fw, fh, fill);
    char buf[32];
    snprintf(buf, sizeof(buf), "%d%%", (int)(pct * 100.0f));
    DrawText(buf, (int)(box.x + w + 10), (int)box.y - 2, 18, RAYWHITE);
}


void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    for (int i = 0; i < NUMBER_OF_TILES; ++i)
    {
        if (i == win_tile_index) continue;
        gTiles[i].render();
    }

    gPlatform.render();
    gTileWin.render();
    gSpaceship->render();

    if (gGameState == GameState::WIN)
    {
        const char* msg = "you win";
        int font_size = 34;
        int text = MeasureText(msg, font_size);
        DrawText(msg, (SCREEN_WIDTH - text)/2, (int)(ORIGIN.y - 40), font_size, GREEN);
    }
    else if (gGameState == GameState::LOSE)
    {
        const char* msg = "You lose";
        int font_size = 34;
        int text = MeasureText(msg, font_size);
        DrawText(msg, (SCREEN_WIDTH - text)/2, (int)(ORIGIN.y - 40), font_size, RED);
    }
    DrawFuelUI();

    EndDrawing();
}

void shutdown()
{
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
