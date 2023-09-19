#include <raylib.h>
#include <raymath.h>

# define FRAMES_SPEED 10
#define TILE_SIZE 50
#define COLUMN 25
#define ROW 16

Texture2D sky, grass, sun, dirt, player, coin;
Vector2 cam_offset;
Rectangle player_source_rect, player_dest_rect;
Sound jump, coin_sound;
Camera2D camera;

int current_frame = 0, frames_counter = 0;
int dx, dy = 0, double_jump = 0;

//world map to draw
int world_data[ROW][COLUMN] = {
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},              // 0 FOR AIR
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},              // 1 FOR DIRT                
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0},              // 2 FOR GRASS        
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},              // 3 FOR COIN                         
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0}, 
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                                {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, 
                                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
                                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
                                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                            };


static void PlayerMovement(int speed);
static void PauseMovement(void);
static void CheckCollision(void);
static void Draw(void);

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
    player_source_rect = (Rectangle){((float)player.width/6)*2, 0.0f, (float)player.width/6, (float)player.height};
    player_dest_rect = (Rectangle){0, 0, TILE_SIZE, TILE_SIZE};

    cam_offset = (Vector2){ScreenWidth/2, (ROW-5)*TILE_SIZE};
    camera = (Camera2D){cam_offset, (Vector2){player_dest_rect.x, 0}, 0, 1};
    
    jump = LoadSound("audio/jump.mp3");
    coin_sound = LoadSound("audio/coin.mp3");
    
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        dx = 0;

        //movement of player (left or right)
        if(IsKeyDown(KEY_RIGHT) && (player_dest_rect.x+TILE_SIZE)<=(TILE_SIZE*COLUMN-cam_offset.x))
        {
           if (player_source_rect.width<0)
                player_source_rect.width *= -1;
            
           PlayerMovement(5);
        }
        else if(IsKeyDown(KEY_LEFT) && player_dest_rect.x>=-cam_offset.x)
        {
           if (player_source_rect.width>0)
                player_source_rect.width *= -1;
            
           PlayerMovement(-5);
        }
        if(IsKeyReleased(KEY_RIGHT) || IsKeyReleased(KEY_LEFT))
            PauseMovement();

        //jumping of player
        if(IsKeyPressed(KEY_SPACE) && (double_jump<2))
        {
            PlaySound(jump);
            dy = -15;
            double_jump++;
            PauseMovement();
        }

        CheckCollision();
        Draw(); 
    }
    //unloading textures/sounds and closing window
    UnloadTexture(sky);
    UnloadTexture(grass);
    UnloadTexture(sun);
    UnloadTexture(dirt);
    UnloadTexture(player);
    UnloadTexture(coin);
    UnloadSound(jump);
    UnloadSound(coin_sound);
    
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}

//function for different animation and collision in x-axis
void PlayerMovement(int speed)
{
   frames_counter++;
   if (frames_counter >= (60/FRAMES_SPEED) && (!double_jump))
    {
        frames_counter = 0;
        current_frame++;
        if (current_frame > 5) 
            current_frame = 0;
        player_source_rect.x = (float)current_frame*(float)player.width/6;
    }
    dx = speed;
}   

//function for pausing animation
void PauseMovement(void)
{
    current_frame = 2;
    player_source_rect.x = (float)current_frame*(float)player.width/6;
}

//checks for collision of the player with the required blocks/elements
void CheckCollision(void)
{
    Rectangle player_pos_screen = 
                                    {
                                        cam_offset.x+player_dest_rect.x,
                                        cam_offset.y+player_dest_rect.y,
                                        player_dest_rect.width,
                                        player_dest_rect.height
                                    };

    if(dy<20)
        dy++;

    for(int i=0;i<ROW;i++)
        for(int j=0;j<COLUMN;j++)
        {
            if(world_data[i][j]==2)
            {
                //check for collision in y direction
                if(CheckCollisionRecs(player_pos_screen, (Rectangle){j*TILE_SIZE, i*TILE_SIZE-dy, TILE_SIZE, TILE_SIZE}))
                {
                    //check if below the ground i.e. jumping
                    if(dy<0)
                    {
                        player_dest_rect.y = (i+1)*TILE_SIZE-cam_offset.y;
                        dy = 0;
                    }
                    //check if above the ground i.e. falling
                    else if(dy>=0)
                    {
                        player_dest_rect.y = (i-1)*TILE_SIZE-cam_offset.y;
                        double_jump = 0;
                        dy = 0;
                    }
                }
                //check for collision in x direction
                if(CheckCollisionRecs(player_pos_screen, (Rectangle){j*TILE_SIZE-dx, i*TILE_SIZE, TILE_SIZE, TILE_SIZE}))
                    dx = 0;
            }
            if(world_data[i][j]==3)
                if(CheckCollisionRecs(player_pos_screen, (Rectangle){j*TILE_SIZE, i*TILE_SIZE+10, coin.width, coin.height}))
                {
                    PlaySound(coin_sound);
                    world_data[i][j]=0;
                }
        }

    //check if player is above window (y<0 in screen)
    if(player_dest_rect.y<-cam_offset.y)
    {
        player_dest_rect.y = -cam_offset.y;
        dy = 0;
    }
    player_dest_rect.y += dy;
    player_dest_rect.x += dx;
}

//drawing on the screen (and update camera)
void Draw(void)
{
    if(player_dest_rect.x>0 && (player_dest_rect.x+cam_offset.x*2)<(TILE_SIZE*COLUMN))
        camera.target = (Vector2){player_dest_rect.x, 0};

    BeginDrawing();
        ClearBackground(SKYBLUE);
        BeginMode2D(camera);
            DrawTextureV(sky, Vector2Negate(cam_offset), WHITE);
            for(int i=0;i<ROW;i++)
                for(int j=0;j<COLUMN;j++)
                {
                    Vector2 pos = {TILE_SIZE*j-cam_offset.x, TILE_SIZE*i-cam_offset.y};
                    if (world_data[i][j]==1)
                        DrawTextureV(dirt, pos, WHITE);
                    else if(world_data[i][j]==2)
                        DrawTextureV(grass, pos, WHITE);
                    else if(world_data[i][j]==3)
                        DrawTextureV(coin, pos, WHITE);
                }
            DrawTexturePro(player, player_source_rect, player_dest_rect, (Vector2){0, 0}, 0, WHITE);
        EndMode2D();
        DrawTexture(sun, TILE_SIZE*2, TILE_SIZE*2, WHITE);
        DrawFPS(10, 10);
    EndDrawing();
}
