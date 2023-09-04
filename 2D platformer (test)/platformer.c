#include "raylib.h"

#define TILE_SIZE 50
#define COLUMN 20
#define ROW 16

Texture2D sky, grass, sun, dirt, player, coin;
Sound jump;

int currentFrame = 0, framesCounter = 0, framesSpeed = 10;
int dx, double_jump = 0, gravity = 0;

//world map to draw
int world_data[ROW][COLUMN] = {
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},              // 0 FOR AIR
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},              // 1 FOR DIRT                
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0},              // 2 FOR GRASS        
                                {0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},                                             
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, 
                                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                            };

static void player_movement(Rectangle *player_source_rect, Rectangle *player_dest_rect, int speed);
static void pause_movement(Rectangle *player_source_rect);
static void check_collision(Rectangle *player_dest_rect);

int main(void)
{
    const int ScreenWidth = 1000;
    const int ScreenHeight = 800;

    InitAudioDevice();
    InitWindow(ScreenWidth, ScreenHeight, "game");
    
    sky = LoadTexture("images/sky.png");
    sun = LoadTexture("images/sun.png");
    dirt = LoadTexture("images/dirt.png");  
    grass = LoadTexture("images/grass.png");
    coin = LoadTexture("images/coin.png");
    
    player = LoadTexture("images/scarfy.png");
    Rectangle player_source_rect = {((float)player.width/6)*2, 0.0f, (float)player.width/6, (float)player.height};
    Rectangle player_dest_rect = {50.0f, (ROW-3)*TILE_SIZE, TILE_SIZE, TILE_SIZE};
    
    jump = LoadSound("audio/jump.mp3");
    
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        dx = 0;

        //movement of player (left or right)
        if(IsKeyDown(KEY_RIGHT) && (player_dest_rect.x + player_dest_rect.width)<=ScreenWidth)
        {
           if (player_source_rect.width<0)
                player_source_rect.width *= -1;
            
           player_movement(&player_source_rect, &player_dest_rect, 5);
        }
        else if(IsKeyDown(KEY_LEFT) && (player_dest_rect.x)>=0)
        {
           if (player_source_rect.width>0)
                player_source_rect.width *= -1;
            
           player_movement(&player_source_rect, &player_dest_rect, -5);
        }
        if(IsKeyReleased(KEY_RIGHT) || IsKeyReleased(KEY_LEFT))
            pause_movement(&player_source_rect);

        //jumping of player
        if(IsKeyPressed(KEY_SPACE) && (double_jump<2))
        {
            PlaySound(jump);
            gravity = -15;
            double_jump++;
            pause_movement(&player_source_rect);
        }

        check_collision(&player_dest_rect);
        
        //drawing on the screen
        BeginDrawing();
            DrawTexture(sky, 0, 0, WHITE);
            DrawTexture(sun, TILE_SIZE*2, TILE_SIZE*2, WHITE);
            for(int i=0;i<ROW;i++)
                for(int j=0;j<COLUMN;j++)
                {
                    if (world_data[i][j]==1)
                        DrawTexture(dirt, j*TILE_SIZE, i*TILE_SIZE, WHITE);
                    else if(world_data[i][j]==2)
                        DrawTexture(grass, j*TILE_SIZE, i*TILE_SIZE, WHITE);
                    else if(world_data[i][j]==3)
                        DrawTexture(coin, j*TILE_SIZE, i*TILE_SIZE+10, WHITE);
                }
            DrawTexturePro(player, player_source_rect, player_dest_rect, (Vector2){0, 0}, 0, WHITE);
            DrawFPS(10, 10);
        EndDrawing();
    }
    //unloading textures/sounds and closing window
    UnloadTexture(sky);
    UnloadTexture(grass);
    UnloadTexture(sun);
    UnloadTexture(dirt);
    UnloadTexture(player);
    UnloadTexture(coin);
    UnloadSound(jump);
    
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}

//function for different animation and collision in x-axis
void player_movement(Rectangle *player_source_rect, Rectangle *player_dest_rect, int speed)
{
   framesCounter++;
   if (framesCounter >= (60/framesSpeed) && (!double_jump))
    {
        framesCounter = 0;
        currentFrame++;
        if (currentFrame > 5) 
            currentFrame = 0;
        player_source_rect->x = (float)currentFrame*(float)player.width/6;
    }
    dx = speed;
}   

//function for pausing animation
void pause_movement(Rectangle *player_source_rect)
{
    currentFrame = 2;
    player_source_rect->x = (float)currentFrame*(float)player.width/6;
}

//checks for collision of the player with the required blocks/elements
void check_collision(Rectangle *player_dest_rect)
{
    if(gravity<20)
        gravity++;

    for(int i=0;i<ROW;i++)
        for(int j=0;j<COLUMN;j++)
        {
            if(world_data[i][j]==2)
            {
                //check for collision in y direction
                if(CheckCollisionRecs(*player_dest_rect, (Rectangle){j*TILE_SIZE, i*TILE_SIZE-gravity, TILE_SIZE, TILE_SIZE}))
                {
                    //check if below the ground i.e. jumping
                    if(gravity<0)
                    {
                        player_dest_rect->y = (i+1)*TILE_SIZE;
                        gravity = 0;
                    }
                    //check if above the ground i.e. falling
                    else if(gravity>=0)
                    {
                        player_dest_rect->y = (i-1)*TILE_SIZE;
                        double_jump = 0;
                        gravity = 0;
                    }
                }
                //check for collision in x direction
                if(CheckCollisionRecs(*player_dest_rect, (Rectangle){j*TILE_SIZE-dx, i*TILE_SIZE, TILE_SIZE, TILE_SIZE}))
                    dx = 0;
            }
            if(world_data[i][j]==3)
                if(CheckCollisionRecs(*player_dest_rect, (Rectangle){j*TILE_SIZE, i*TILE_SIZE+10, coin.width, coin.height}))
                {
                    world_data[i][j]=0;
                }
        }

    //check if player is above window (y<0)
    if(player_dest_rect->y<0)
    {
        player_dest_rect->y = 0;
        gravity = 0;
    }
    player_dest_rect->y += gravity;
    player_dest_rect->x += dx;
}
