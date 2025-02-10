#include <raylib.h>
#include "map_parser.h"

#define FRAMES_SPEED 10
#define TOTAL_LEVELS 4
#define TILE_SIZE 50

typedef enum GameState {
    GAME_ACTIVE,
    GAME_PAUSED
} GameState;

Texture2D grass, dirt, player, coin;
Vector2 cam_offset;
Rectangle player_source_rect, player_dest_rect;
Sound jump, coin_sound;
Camera2D camera;
Map map = {.level = 0};
GameState game_state = GAME_ACTIVE;

const int ScreenWidth = 1000, ScreenHeight = 800;
int current_frame = 0, frames_counter = 0;
int dx, dy = 0, double_jump = 0;

static void PlayerMovement(int speed);
static void PauseMovement(void);
static void CheckCollision(void);
static void Draw(void);
static void UnloadData(void);

int main(void)
{
    SetTraceLogLevel(LOG_NONE);
    LoadMapData(&map);
    if(map.data==NULL)
    {
        UnloadMapData(&map);
        return 1;
    }

    InitAudioDevice();
    InitWindow(ScreenWidth, ScreenHeight, "game");
    
    dirt = LoadTexture("resources/images/dirt.png");  
    grass = LoadTexture("resources/images/grass.png");
    coin = LoadTexture("resources/images/coin.png");
    
    player = LoadTexture("resources/images/scarfy.png");
    player_source_rect = (Rectangle){((float)player.width/6)*2, 0.0f, (float)player.width/6, (float)player.height};
    player_dest_rect = (Rectangle){0, 0, TILE_SIZE, TILE_SIZE};

    cam_offset = (Vector2){ScreenWidth/2, (map.row-5)*TILE_SIZE};
    camera = (Camera2D){cam_offset, (Vector2){player_dest_rect.x, 0}, 0, 1};
    
    jump = LoadSound("resources/audio/jump.mp3");
    coin_sound = LoadSound("resources/audio/coin.mp3");
    
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        if(game_state==GAME_ACTIVE)
        {
            dx = 0;

            if(IsKeyPressed(KEY_L) && map.level<TOTAL_LEVELS)
            {
                LoadMapData(&map);
                if(map.data==NULL)
                {
                    UnloadData();
                    return 1;
                }
                player_dest_rect.x = 0;
                player_dest_rect.y = 0;
                if(player_source_rect.width<0)
                    player_source_rect.width *= -1;
                camera.target = (Vector2){player_dest_rect.x, 0};
            }

            //Movement of the player (left or right)
            if(IsKeyDown(KEY_RIGHT) && (player_dest_rect.x+TILE_SIZE)<=(TILE_SIZE*map.column-cam_offset.x))
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

            //Jumping of the player
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

        else if(game_state==GAME_PAUSED)
        {
            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("PAUSED", 200, 200, 70, RED);
            EndDrawing();
        }

        if(IsKeyPressed(KEY_P))
        {
            switch (game_state)
            {
                case GAME_ACTIVE:
                    game_state = GAME_PAUSED;
                    break;
                case GAME_PAUSED:
                    game_state = GAME_ACTIVE;
                    break;
            }
        }
    }
    UnloadData();
    
    return 0;
}

//Function for showing frames of the player
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

//Function for stop showing frames of the player
void PauseMovement(void)
{
    current_frame = 2;
    player_source_rect.x = (float)current_frame*(float)player.width/6;
}

//Checks for collision of the player with the required blocks/elements
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

    for(int i=0;i<map.row;i++)
        for(int j=0;j<map.column;j++)
        {
            if(map.data[i*map.column+j]=='2')
            {
                //Check for collision in y direction
                if(CheckCollisionRecs(player_pos_screen, (Rectangle){j*TILE_SIZE, i*TILE_SIZE-dy, TILE_SIZE, TILE_SIZE}))
                {
                    //Check if below the ground i.e. jumping
                    if(dy<0)
                    {
                        player_dest_rect.y = (i+1)*TILE_SIZE-cam_offset.y;
                        dy = 0;
                    }
                    //Check if above the ground i.e. falling
                    else
                    {
                        player_dest_rect.y = (i-1)*TILE_SIZE-cam_offset.y;
                        double_jump = 0;
                        dy = 0;
                    }
                }
                //Check for collision in x direction
                if(CheckCollisionRecs(player_pos_screen, (Rectangle){j*TILE_SIZE-dx, i*TILE_SIZE, TILE_SIZE, TILE_SIZE}))
                    dx = 0;
            }
            if(map.data[i*map.column+j]=='3')
                if(CheckCollisionRecs(player_pos_screen, (Rectangle){j*TILE_SIZE, i*TILE_SIZE+10, coin.width, coin.height}))
                {
                    PlaySound(coin_sound);
                    map.data[i*map.column+j]=0;
                }
        }

    //Check if player is above window (y<0 in screen)
    if(player_dest_rect.y<(-cam_offset.y))
    {
        player_dest_rect.y = -cam_offset.y;
        dy = 0;
    }
    player_dest_rect.y += dy;
    player_dest_rect.x += dx;
}

//Drawing on the screen (and update camera)
void Draw(void)
{
    if(player_dest_rect.x>0 && (player_dest_rect.x+cam_offset.x*2)<(TILE_SIZE*map.column))
        camera.target = (Vector2){player_dest_rect.x, 0};

    BeginDrawing();
        ClearBackground(SKYBLUE);
        BeginMode2D(camera);
            for(int i=0;i<map.row;i++)
                for(int j=0;j<map.column;j++)
                {
                    Vector2 pos = {TILE_SIZE*j-cam_offset.x, TILE_SIZE*i-cam_offset.y};
                    if (map.data[i*map.column+j]=='1')
                        DrawTextureV(dirt, pos, WHITE);
                    else if(map.data[i*map.column+j]=='2')
                        DrawTextureV(grass, pos, WHITE);
                    else if(map.data[i*map.column+j]=='3')
                        DrawTextureV(coin, pos, WHITE);
                }
            DrawTexturePro(player, player_source_rect, player_dest_rect, (Vector2){0, 0}, 0, WHITE);
        EndMode2D();
        DrawFPS(10, 10);
        DrawText(TextFormat("LEVEL: %d", map.level), ScreenWidth/2, 10, 30, RED);
    EndDrawing();
}

//Unloading data and closing window
void UnloadData(void)
{
    UnloadTexture(grass);
    UnloadTexture(dirt);
    UnloadTexture(player);
    UnloadTexture(coin);
    UnloadSound(jump);
    UnloadSound(coin_sound);
    UnloadMapData(&map);
    
    CloseAudioDevice();
    CloseWindow();
}
