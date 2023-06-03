//still in development, not recommended to use
#include "raylib.h"

#define TILE_SIZE 50
#define COLUMN 20
#define ROW 16

Texture2D sky, grass, sun, dirt, player;
int currentFrame = 0, framesCounter = 0, framesSpeed = 10, double_jump = 0, gravity = 0;
bool collided_l = false, collided_r = false, jumped = false;

void player_movement(Rectangle *player_source_rect, Rectangle *player_dest_rect, int speed);
void pause_movement(Rectangle *player_source_rect);
void y_collision(Rectangle *player_dest_rect, Rectangle *grass_dest_rect, int i);

int main()
{
    const int ScreenWidth = 1000;
    const int ScreenHeight = 800;
    
    //world map to draw
    int world_data[][COLUMN] = {
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},              // 0 FOR AIR
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},              // 1 FOR DIRT                
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},              // 2 FOR GRASS        
                                    {0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0},                                             
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0}, 
                                    {0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                    {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                    {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0}, 
                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, 
                                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                                };
    
    InitAudioDevice();
    InitWindow(ScreenWidth, ScreenHeight, "game");
    
    sky = LoadTexture("images/sky.png");
    sun = LoadTexture("images/sun.png");
    
    dirt = LoadTexture("images/dirt.png");  
    
    grass = LoadTexture("images/grass.png");
    Rectangle grass_source_rect = {0, 0, grass.width, grass.height};
    Rectangle grass_dest_rect = {0, 0, grass.width, grass.height};
    
    player = LoadTexture("images/scarfy.png");
    Rectangle player_source_rect = {((float)player.width/6)*2, 0.0f, (float)player.width/6, (float)player.height};
    Rectangle player_dest_rect = {50.0f, (ROW-3)*TILE_SIZE+1, TILE_SIZE, TILE_SIZE};
    
    Sound jump = LoadSound("audio/jump.mp3");
    
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        //movement of player (left or right)
        if(IsKeyDown(KEY_RIGHT) && (player_dest_rect.x + player_dest_rect.width)<=ScreenWidth && !collided_r)
        {
           if (player_source_rect.width<0)
                player_source_rect.width *= -1;
            
           player_movement(&player_source_rect, &player_dest_rect, 5);
        }
        else if(IsKeyDown(KEY_LEFT) && (player_dest_rect.x)>=0 && !collided_l)
        {
           if (player_source_rect.width>0)
                player_source_rect.width *= -1;
            
           player_movement(&player_source_rect, &player_dest_rect, -5);
        }
        if(IsKeyReleased(KEY_RIGHT) || IsKeyReleased(KEY_LEFT))
            pause_movement(&player_source_rect);
        
        collided_r = false;
        collided_l = false;

        //jumping of player
        if(IsKeyPressed(KEY_SPACE) && (double_jump<2))
        {
            PlaySound(jump);
            gravity = -15;
            double_jump++;
            pause_movement(&player_source_rect);
        }
        
        gravity++;
        player_dest_rect.y += gravity;
        
        //drawing on the screen
        BeginDrawing();
            DrawTexture(sky, 0, 0, WHITE);
            DrawTexture(sun, 100, 100, WHITE);
            for(int i=0;i<ROW;i++)
            {
                for(int j=0;j<COLUMN;j++)
                {
                    y_collision(&player_dest_rect, &grass_dest_rect, i);
                    if (world_data[i][j]==1)
                        DrawTexture(dirt, j*TILE_SIZE, i*TILE_SIZE, WHITE);
                    else if(world_data[i][j]==2)
                    {
                        grass_dest_rect.x = j*TILE_SIZE;
                        grass_dest_rect.y = i*TILE_SIZE;
                        DrawTexturePro(grass, grass_source_rect, grass_dest_rect, (Vector2){0, 0}, 0, WHITE);
                    }
                }
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
    UnloadSound(jump);
    
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}

//function for different animation
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
    player_dest_rect->x += speed;
}   

//function for pausing animation
void pause_movement(Rectangle *player_source_rect)
{
    currentFrame = 2;
    player_source_rect->x = (float)currentFrame*(float)player.width/6;
}

//checking for collision of player with block in y direction
void y_collision(Rectangle *player_dest_rect, Rectangle *grass_dest_rect, int i)
{
    if(CheckCollisionRecs(*player_dest_rect, *grass_dest_rect))
    {
        if (gravity<0)
        {
            player_dest_rect->y = (i+1)*TILE_SIZE;
            gravity = 0;
        }
        else if (gravity>0) 
        {
            player_dest_rect->y = (i-1)*TILE_SIZE+1;
            gravity = double_jump = 0;
        }
    }
}
