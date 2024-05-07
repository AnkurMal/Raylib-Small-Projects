#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <stdlib.h>

#define SIZE 40
#define FRAME_RATE 7
#define SAP_GREEN (Color){149, 232, 78, 255}

#define screenWidth 960
#define screenHeight 680
#define leftOffset SIZE
#define upperOffset SIZE

#define defaultTexture(width, height) {rlGetTextureIdDefault(), width, height, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

bool gameActive = false;

const int rightOffset = screenWidth-SIZE*2, lowerOffset = screenHeight-SIZE*2;
const int totalLenth = ((screenWidth-SIZE*2)*(screenHeight-SIZE*2))/(SIZE*SIZE);
int xMov, yMov, framesCounter = 0, snakeLength, highestScore = 0;

Vector2 *snakePos, foodPos = {0};
Sound food, hit, move;

void movement(int xSteps, int ySteps);
void initSnakeAndFoodPos(void);
void regenerateFood(void);

int main(void)
{    
    SetTraceLogLevel(LOG_NONE);
    InitAudioDevice();
    InitWindow(screenWidth, screenHeight, "Snake");
    
    snakePos = malloc(totalLenth*sizeof(Vector2)); 

    Shader shader = LoadShader(0, "shader.fs");
    Texture2D texture = defaultTexture(SIZE, SIZE);
    
    Image icon = LoadImage("images/snake_icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
    
    food = LoadSound("audio/food.mp3");
    hit = LoadSound("audio/hit.mp3");
    move = LoadSound("audio/move.mp3");
    
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        if(gameActive)
        {
            bool foodNeeded = false;

            if(IsKeyPressed(KEY_DOWN) && !yMov)
                movement(0, SIZE);
            else if(IsKeyPressed(KEY_UP) && !yMov)
                movement(0, -SIZE);
            else if(IsKeyPressed(KEY_RIGHT) && !xMov)
                movement(SIZE, 0);
            else if(IsKeyPressed(KEY_LEFT) && !xMov)
                movement(-SIZE, 0);

            if(Vector2Equals(snakePos[0], foodPos)  && snakeLength!=totalLenth)
            {
                PlaySound(food);
                framesCounter = 60/FRAME_RATE;
                foodNeeded = true;
                ++snakeLength;
            }

            framesCounter++;
            if(framesCounter >= 60/FRAME_RATE)
            {
                framesCounter = 0;
                
                for(int i=snakeLength-1; i>=1; i--)
                {
                    snakePos[i].x = snakePos[i-1].x;
                    snakePos[i].y = snakePos[i-1].y;
                }
                snakePos[0].x += xMov;
                snakePos[0].y += yMov;
            }
            if(foodNeeded) regenerateFood();
            
            for(int i=1; i<snakeLength; i++)
                if(Vector2Equals(snakePos[i], snakePos[0]))
                {
                    gameActive = false;
                    break;
                }
            
            if(snakePos[0].x<leftOffset || snakePos[0].x>rightOffset || snakePos[0].y<upperOffset
               || snakePos[0].y>lowerOffset)
                gameActive = false;
            
            if(gameActive)
            {
                BeginDrawing();
                    ClearBackground(SAP_GREEN);
                    
                    DrawTextureV(texture, foodPos, RED);
                    BeginShaderMode(shader);
                        for(int i=0; i<snakeLength; i++)
                            DrawTextureV(texture, snakePos[i], (i)? DARKGRAY:BLACK);
                    EndShaderMode();
                    
                    int counter = screenWidth/SIZE;
                    for(int i=1; i<counter; i++)
                    {
                        if(i*SIZE>upperOffset && i*SIZE<=lowerOffset)
                            DrawLine(leftOffset, SIZE*i, rightOffset+SIZE, SIZE*i, BLACK);
                        if(i*SIZE>leftOffset && i*SIZE<=rightOffset)
                            DrawLine(SIZE*i, upperOffset, SIZE*i, lowerOffset+SIZE, BLACK);
                    }
                    DrawRectangleLinesEx((Rectangle){leftOffset, upperOffset, rightOffset, lowerOffset}, 3, BLACK);
                    DrawText(TextFormat("Score: %d", snakeLength-1), leftOffset, 5, 30, BLACK);
                EndDrawing();
            }
            else
                PlaySound(hit);
        }
        else
        {
            if(snakeLength-1>highestScore)
                highestScore = snakeLength-1;
            
            BeginDrawing();
                ClearBackground(SAP_GREEN);
                if(!foodPos.x)
                {
                    DrawText("SNAKE GAME", 220, 220, 80, RED);
                    DrawText("Press space to play...", 270, 320, 40, VIOLET);
                }
                else
                {
                    if(snakeLength!=totalLenth)
                        DrawText("GAME OVER!", 280, 150, 70, RED);
                    else
                        DrawText("YOU WON!", 280, 150, 70, RED);
                    DrawText(TextFormat("Score: %d", snakeLength-1), 280, 300, 50, BLACK);
                    DrawText(TextFormat("Highest Score: %d", highestScore), 280, 370, 50, BLACK);
                    DrawText("Press space to continue...", 220, 500, 40, VIOLET);
                }
            EndDrawing();
            
            if(IsKeyPressed(KEY_SPACE))
            {
                xMov = yMov = 0;
                snakeLength = 1;
                initSnakeAndFoodPos();
                gameActive = true;
            }
        }
    }
    free(snakePos);
    UnloadSound(food);
    UnloadSound(hit);
    UnloadSound(move);
    UnloadTexture(texture);
    UnloadShader(shader);
    
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}

void movement(int xSteps, int ySteps)
{   
    PlaySound(move);
    xMov = xSteps;
    yMov = ySteps;
}

void regenerateFood(void)
{
    bool foodNotGenerated = true;
    while(foodNotGenerated)
    {   
        foodNotGenerated = false;
        foodPos.x = GetRandomValue(leftOffset/SIZE, rightOffset/SIZE)*SIZE;
        foodPos.y = GetRandomValue(upperOffset/SIZE, lowerOffset/SIZE)*SIZE;
        
        for(int i=0; i<snakeLength; i++)
            if(Vector2Equals(snakePos[i], foodPos))
            {
                foodNotGenerated = true;
                break;
            }
    }
}

void initSnakeAndFoodPos(void)
{
    snakePos[0].x = GetRandomValue(leftOffset/SIZE, rightOffset/SIZE)*SIZE;
    snakePos[0].y = GetRandomValue(upperOffset/SIZE, lowerOffset/SIZE)*SIZE;
    
    regenerateFood();
}
