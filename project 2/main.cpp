/**
* Author: Hero Zhang
* Assignment: Pong Clone
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1600 / 2,
              SCREEN_HEIGHT = 900 / 2,
              FPS           = 60,
              SIZE          = 500 / 2,
              SPEED         = 200;

constexpr float ball_speed = 80.0f;   

constexpr char    BG_COLOUR[]    = "#000000ff";
constexpr Vector2 ORIGIN         = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  BASE_SIZE      = { (float) SIZE, (float) SIZE },
                  INIT_POS  = { ORIGIN.x - 320.0f, ORIGIN.y };
float gScaleFactor   = 100.0f;
enum Status {PLAY,GAMEOVER};
enum Mode {SINGLE,DOUBLE};
Status gStatus = PLAY;
Mode gMode = {DOUBLE};
enum Singlepaddle{UP,DOWN};
Singlepaddle gSinglepaddle = UP;
int gWinner = 0;
struct Ball { Vector2 pos, vel; bool active; };
constexpr int MAX_BALLS = 3;
int gActiveBalls = 1;           
Ball gBalls[MAX_BALLS]; 

constexpr char left_win[]  = "assets/game/left_win.png";
constexpr char right_win[] = "assets/game/right_win.png";

Texture2D gleftwinTexture;
Texture2D grightwinTexture;

constexpr char paddle1[] = "assets/game/paddle.png";
constexpr char paddle2[] = "assets/game/paddle.png";
constexpr char ball[] = "assets/game/ball.png";

Texture2D gpaddle1Texture;
Texture2D gpaddle2Texture;
Texture2D gballTexture;

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f;

Vector2 gPosition1  = INIT_POS,
        gMovement1  = { 0.0f, 0.0f },        
        gMovement2  = { 0.0f, 0.0f },

        gScale     = { 30.0f, 72.0f },
        gBallScale = {16.0f, 16.0f},
        gPosition2  = {ORIGIN.x + 320.0f, ORIGIN.y};

unsigned int startTime;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 * 
 * @see 
 * 
 * @param positionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
bool isColliding(const Vector2 *positionA,  const Vector2 *scaleA,
                 const Vector2 *positionB,  const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);
    return (xDistance < 0.0f && yDistance < 0.0f);
}


void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture, 
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}
void resetBalls(int count) {
    gActiveBalls = count;
    Vector2 directions[3] = { {  1.0f, -0.6f }, { -1.0f, 0.45f }, { 1.0f, 0.9f } };
    for (int i = 0; i < MAX_BALLS; ++i) {
        gBalls[i].pos = ORIGIN;

        Vector2 d = directions[i];
        Normalise(&d);                
        d.x *= ball_speed;
        d.y *= ball_speed;
        gBalls[i].vel = d;

        gBalls[i].active = (i < gActiveBalls);
    }
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "User Input / Collision Detection");

    startTime = time(NULL);

    gpaddle1Texture  = LoadTexture(paddle1);
    gpaddle2Texture  = LoadTexture(paddle2);
    gballTexture = LoadTexture(ball);

    gleftwinTexture  = LoadTexture(left_win);
    grightwinTexture = LoadTexture(right_win);
    resetBalls(1);

    SetTargetFPS(FPS);
}

void processInput() 
{
    gMovement1 = { 0.0f, 0.0f };
    gMovement2 = { 0.0f, 0.0f };
    if (IsKeyPressed(KEY_T)) {
        gMode = (gMode == DOUBLE) ? SINGLE : DOUBLE;
    }

    if (gStatus == PLAY) {
        if      (IsKeyDown(KEY_W)) gMovement1.y = -1;
        else if (IsKeyDown(KEY_S)) gMovement1.y =  1;

        if (gMode == DOUBLE) {
            if      (IsKeyDown(KEY_UP))   gMovement2.y = -1;
            else if (IsKeyDown(KEY_DOWN)) gMovement2.y =  1;
        }
    }
    if (gStatus == PLAY) {
    if (IsKeyPressed(KEY_ONE))   resetBalls(1);
    if (IsKeyPressed(KEY_TWO))   resetBalls(2);
    if (IsKeyPressed(KEY_THREE)) resetBalls(3);
}
    if (GetLength(&gMovement1) > 1.0f)  Normalise(&gMovement1);
    if (GetLength(&gMovement2) > 1.0f) Normalise(&gMovement2);
}

void update() 
{
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    if (gStatus == GAMEOVER) {
        return;
    }
    gPosition1 = {
        gPosition1.x + SPEED * gMovement1.x * deltaTime,
        gPosition1.y + SPEED * gMovement1.y * deltaTime
    };
    gPosition2 = {
        gPosition2.x + SPEED * gMovement2.x * deltaTime,
        gPosition2.y + SPEED * gMovement2.y * deltaTime
    };
        if (gPosition1.y < gScale.y * 0.5f)                 gPosition1.y = gScale.y * 0.5f;
        if (gPosition1.y > SCREEN_HEIGHT - gScale.y * 0.5f) gPosition1.y = SCREEN_HEIGHT - gScale.y * 0.5f;
        if (gPosition2.y < gScale.y * 0.5f)                 gPosition2.y = gScale.y * 0.5f;
        if (gPosition2.y > SCREEN_HEIGHT - gScale.y * 0.5f) gPosition2.y = SCREEN_HEIGHT - gScale.y * 0.5f;

    if (gMode == SINGLE && gStatus == PLAY) {
        if (gSinglepaddle == UP) {
            gPosition2.y -= SPEED * deltaTime;
            if (gPosition2.y <= gScale.y * 0.5f + 5.0f) {
                gPosition2.y = gScale.y * 0.5f + 5.0f;
                gSinglepaddle = DOWN;
            }
        } else {
            gPosition2.y += SPEED * deltaTime;
            if (gPosition2.y >= SCREEN_HEIGHT - gScale.y * 0.5f - 5.0f) {
                gPosition2.y = SCREEN_HEIGHT - gScale.y * 0.5f - 5.0f;
                gSinglepaddle = UP;
            }
        }
    }

    for (int i = 0; i < gActiveBalls; ++i) {
        Ball &b = gBalls[i];
        if (!b.active) continue;

        b.pos.x += b.vel.x * deltaTime;
        b.pos.y += b.vel.y * deltaTime;

        if (b.pos.y - gBallScale.y * 0.5f <= 0.0f) {
            b.vel.y = -b.vel.y;      
            b.pos.y = gBallScale.y * 0.5f;       
        }
        else if (b.pos.y + gBallScale.y * 0.5f >= SCREEN_HEIGHT) {
            b.vel.y = -b.vel.y;     
            b.pos.y = SCREEN_HEIGHT - gBallScale.y * 0.5f; 
        }

        if (b.vel.x < 0.0f &&
            isColliding(&b.pos, &gBallScale, &gPosition1, &gScale)) {
            b.pos.x = gPosition1.x + (gScale.x * 0.5f + gBallScale.x * 0.5f);
            b.vel.x *= -1.0f;
        }

        if (b.vel.x > 0.0f &&
            isColliding(&b.pos, &gBallScale, &gPosition2, &gScale)) {
            b.pos.x = gPosition2.x - (gScale.x * 0.5f + gBallScale.x * 0.5f);
            b.vel.x *= -1.0f;
        }

        float halfBx = gBallScale.x * 0.5f;
        if (b.pos.x + halfBx < 0.0f)          { gStatus = GAMEOVER; gWinner = +1; break; }
        if (b.pos.x - halfBx > SCREEN_WIDTH)  { gStatus = GAMEOVER; gWinner = -1; break; }
    }

}
   
void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    renderObject(&gpaddle1Texture, &gPosition1, &gScale);
    renderObject(&gpaddle2Texture, &gPosition2, &gScale);

    for (int i = 0; i < gActiveBalls; ++i) {
        const Ball &b = gBalls[i];
        Rectangle source = { 
            0,0,
            (float)gballTexture.width,
            (float)gballTexture.height };

        Rectangle destination = { 
            b.pos.x, 
            b.pos.y, 
            gBallScale.x, 
            gBallScale.y };

        Vector2   org = { gBallScale.x * 0.5f, gBallScale.y * 0.5f };
        DrawTexturePro(gballTexture, source, destination, org, 0.0f, WHITE);
    }

    if (gStatus == GAMEOVER) {
        const Texture2D& banner = (gWinner > 0) ? grightwinTexture : gleftwinTexture;

        float maxW = SCREEN_WIDTH  * 0.8f;
        float maxH = SCREEN_HEIGHT * 0.25f;

        float sx = maxW / banner.width;
        float sy = maxH / banner.height;
        float s  = (sx < sy ? sx : sy);

        float drawW = banner.width  * s;
        float drawH = banner.height * s;

        Rectangle src = {
            0,0, 
            (float)banner.width, 
            (float)banner.height };

        Rectangle dst = { 
            SCREEN_WIDTH * 0.5f, 
            SCREEN_HEIGHT * 0.5f, 
            drawW, 
            drawH };
        Vector2   org = { drawW * 0.5f, drawH * 0.5f };

        DrawTexturePro(banner, src, dst, org, 0.0f, WHITE);
    }
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