/**
* Author: [Hero Zhang]
* Assignment: Simple 2D Scene
* Date due: 2025-09-29, 11:59pm (extension)
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "CS3113/cs3113.h"
#include <math.h>

// Global Constants
constexpr int   SCREEN_WIDTH  = 1300,
                SCREEN_HEIGHT = 800,
                FPS           = 60,
                SIZE          = 200;
constexpr float LIMIT_Vertical = 30.0f;
constexpr float MAX_AMP       = 10.0f;
constexpr float RADIUS          = 200.0f, // radius of the orbit
                ORBIT_SPEED     = 0.05f,  // the speed at which the triangle will travel its orbit
                PULSE_SPEED     = 100.0f, // how fast the triangle is going to be "pulsing"
                PULSE_INCREMENT = 10.0f;  // the current value we're scaling by
constexpr float Zombie_Pos_x = 300.0f;
constexpr float Zombie_Pos_y = 0.0f;

constexpr Color BG_Color[] = {RED, WHITE, BLUE, GREEN};
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 BASE_SIZE   = { (float) SIZE, (float) SIZE };

constexpr char squash[]     = "squash.png";
constexpr char zombie[]     = "zombie.png";
constexpr char sunflower[]  = "sunflower.png";

// Global Variables
AppStatus gAppStatus           = RUNNING;
float     gScaleFactor         = SIZE,
          gAngle               = 0.0f,
          gPulseTime           = 0.0f;
Vector2   gPosition_squash     = {ORIGIN.x-300,ORIGIN.y},
          gPosition_zombie     = {ORIGIN.x+300,ORIGIN.y},
          gPosition_sunflower  = ORIGIN,
          gScale_squash        = BASE_SIZE,
          gScale_sunflower     = BASE_SIZE,
          gScale_zombie        = BASE_SIZE;

constexpr float gPosition_squash_x  = SCREEN_WIDTH / 2 -300;
constexpr float gPosition_squash_y  = SCREEN_HEIGHT / 2;
constexpr float height_triangle = 100.0f;
constexpr float base_triangle = 200.0f;
constexpr float vertical_max = 5.0f;
constexpr float horizontal_max = 50.0f;
float gDirection_zombie_x = 1.0f;
float gDirection_zombie_y = -1.0f;
constexpr float gPosition_zombie_x  = SCREEN_WIDTH / 2 +300;
constexpr float gPosition_zombie_y  = SCREEN_HEIGHT / 2;

int triangle_stage = 0;
int count_frame = 0;
int current_color = 0;
float gAngle_squash = 0.0f;

float     gPreviousTicks = 0.0f;

Texture2D gTexture_squash;
Texture2D gTexture_zombie;
Texture2D gTexture_sunflower;


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures");

    gTexture_squash = LoadTexture(squash);
    gTexture_zombie = LoadTexture(zombie);
    gTexture_sunflower = LoadTexture(sunflower);


    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    gPulseTime += 5.0f * deltaTime;

    //the squash rotate by itself and translate in a triangle motion
    gAngle_squash = gAngle_squash + 3;
    if (triangle_stage == 0){
        gPosition_squash.x += 1.0f;
        gPosition_squash.y -= 1.0f;
        if ((gPosition_squash.x >= base_triangle + gPosition_squash_x) &&
        (gPosition_squash.y <= gPosition_squash_y - height_triangle)){
            triangle_stage = 1;
        }
    }else if (triangle_stage == 1){
        gPosition_squash.x += 1.0f;
        gPosition_squash.y += 1.0f;
        if ((gPosition_squash.x > base_triangle + gPosition_squash_x) &&
        (gPosition_squash.y == gPosition_squash_y)){
            triangle_stage = 2;
        }
    }else if (triangle_stage == 2){
        gPosition_squash.x -= 1.0f;
        if ((gPosition_squash.x <= gPosition_squash_x) &&
        (gPosition_squash.y <= gPosition_squash_y)){
            triangle_stage = 0;
        }
    }

    //the sunflower move in an ordit around the squash
    gPosition_sunflower.x  = gPosition_squash.x + RADIUS * cos(gPulseTime);
    gPosition_sunflower.y  = gPosition_squash.y + RADIUS * sin(gPulseTime);

    //movement for the zombie
    gScale_zombie = {
        BASE_SIZE.x + MAX_AMP * cos(gPulseTime),
        BASE_SIZE.y + MAX_AMP * cos(gPulseTime)
    };

    gPosition_zombie.x += gDirection_zombie_x;
    gPosition_zombie.y += gDirection_zombie_y;

    if (gPosition_zombie.x > gPosition_zombie_x + horizontal_max){
        gDirection_zombie_x = -1.0f;
    }
    else if (gPosition_zombie.x < gPosition_zombie_x - horizontal_max){
        gDirection_zombie_x = 1.0f;
    }

    if (gPosition_zombie.y < gPosition_zombie_y - vertical_max){
        gDirection_zombie_y = 1.0f;
    }
    else if (gPosition_zombie.y > gPosition_zombie_y + vertical_max){
        gDirection_zombie_y = -1.0f;
    }

    //I want to change color every 40 frame;
    count_frame++;
    if (count_frame % 40 ==0){
        current_color += 1;
        if (current_color == 4){
            current_color = 0;
        }
    }
}
void render()
{
    BeginDrawing();
    ClearBackground(BG_Color[current_color]);

    // Whole texture (UV coordinates)
    Rectangle textureArea_squash = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gTexture_squash.width),
        static_cast<float>(gTexture_squash.height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea_squash = {
        gPosition_squash.x,
        gPosition_squash.y,
        static_cast<float>(gScale_squash.x),
        static_cast<float>(gScale_squash.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 objectOrigin_squash = {
        static_cast<float>(gScale_squash.x) / 2.0f,
        static_cast<float>(gScale_squash.y) / 2.0f
    };

    // Whole texture (UV coordinates)
    Rectangle textureArea_zombie = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gTexture_zombie.width),
        static_cast<float>(gTexture_zombie.height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea_zombie = {
        gPosition_zombie.x,
        gPosition_zombie.y,
        static_cast<float>(gScale_zombie.x),
        static_cast<float>(gScale_zombie.y)
    };
    Vector2 objectOrigin_zombie = {
        static_cast<float>(gScale_zombie.x) / 2.0f,
        static_cast<float>(gScale_zombie.y) / 2.0f
    };

    // Whole texture (UV coordinates)
    Rectangle textureArea_sunflower = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gTexture_sunflower.width),
        static_cast<float>(gTexture_sunflower.height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea_sunflower = {
        gPosition_sunflower.x,
        gPosition_sunflower.y,
        static_cast<float>(gScale_sunflower.x),
        static_cast<float>(gScale_sunflower.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 objectOrigin_sunflower = {
        static_cast<float>(gScale_sunflower.x) / 2.0f,
        static_cast<float>(gScale_sunflower.y) / 2.0f
    };
    // Render the texture on screen
    DrawTexturePro(
        gTexture_squash,
        textureArea_squash, 
        destinationArea_squash, 
        objectOrigin_squash, 
        gAngle_squash, 
        WHITE
    );
    DrawTexturePro(
        gTexture_zombie, 
        textureArea_zombie,
        destinationArea_zombie,
        objectOrigin_zombie, 
        gAngle, 
        WHITE
    );
    DrawTexturePro(
        gTexture_sunflower, 
        textureArea_sunflower,
        destinationArea_sunflower,
        objectOrigin_sunflower, 
        gAngle, 
        WHITE
    );

    EndDrawing();
}

void shutdown() { CloseWindow(); }

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