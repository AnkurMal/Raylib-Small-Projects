#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#define FLAPPY_BLUE (Color){51, 165, 255, 255}

Rectangle get_rect(int x, int y, Texture2D sprite)
{
    return (Rectangle){x, y, sprite.width, sprite.height};
}

void pipe_func(Rectangle *pipeup, Rectangle *pipedown, int x)
{
    int y_cord = -((rand()%(630-290+1))+250);
    y_cord -= (*pipeup).y;
    (*pipeup).y += y_cord;
    (*pipedown).y += y_cord;
    (*pipeup).x = x;
    (*pipedown).x = x;   
}

int main()
{
    int score = 0, highest_score = 0;
    float bird_grvity = 0.0f;
    bool game_active = true;
    srand(time(0));
    
    const int screenWidth = 550;
    const int screenHeight = 800;
    
    InitWindow(screenWidth, screenHeight, "Flappy Bird");
    InitAudioDevice();
    
    Texture2D background = LoadTexture("images/background.png");
    
    Texture2D surface = LoadTexture("images/ground.png");
    Rectangle surface_source_rect = get_rect(0, 0, surface);
    Rectangle surface_dest_rect = get_rect(0, 640, surface);
    
    Texture2D flappy = LoadTexture("images/flappy_bird.png");
    Rectangle flappy_source_rect = get_rect(0, 0, flappy);
    Rectangle flappy_dest_rect = get_rect(10, 10, flappy);
    
    Texture2D pipe = LoadTexture("images/pipe.png");   
    Rectangle pipeup_source_rect = {0, 0, pipe.width, -pipe.height};    
    Rectangle pipedown_source_rect = get_rect(0, 0, pipe);
    
    Rectangle pipeup1_dest_rect = get_rect(250, -430, pipe);
    Rectangle pipedown1_dest_rect = get_rect(250, 400, pipe);
    
    Rectangle pipeup2_dest_rect = get_rect(570, -330, pipe);
    Rectangle pipedown2_dest_rect = get_rect(570, 500, pipe);
    
    Sound wing_sound = LoadSound("audio/sfx_wing.wav");
    Sound score_sound = LoadSound("audio/sfx_point.wav");
    Sound hit_sound = LoadSound("audio/sfx_hit.wav");
    
    Image icon = LoadImage("images/flappy_bird.png");
    SetWindowIcon(icon);
    
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if(game_active)
            {
                PlaySound(wing_sound);
                bird_grvity = -7.0;
            }
            else if(!game_active && IsKeyPressed(KEY_SPACE))
            {
                bird_grvity = 0;
                score = 0;
                flappy_dest_rect.y = 10;
                game_active = true;
                pipe_func(&pipeup1_dest_rect, &pipedown1_dest_rect, 250);
                pipe_func(&pipeup2_dest_rect, &pipedown2_dest_rect, 570);
            }
        }
   
        if (game_active)
        {       
            bird_grvity += 0.5;
            flappy_dest_rect.y += bird_grvity;
            
            BeginDrawing();
                DrawTexture(background, 0, 0, WHITE);
                DrawTexturePro(flappy, flappy_source_rect, flappy_dest_rect, (Vector2){0, 0}, 0, WHITE);
                DrawTexturePro(pipe, pipeup_source_rect, pipeup1_dest_rect, (Vector2){0, 0}, 0, WHITE);
                DrawTexturePro(pipe, pipedown_source_rect, pipedown1_dest_rect, (Vector2){0, 0}, 0, WHITE);
                DrawTexturePro(pipe, pipeup_source_rect, pipeup2_dest_rect, (Vector2){0, 0}, 0, WHITE);
                DrawTexturePro(pipe, pipedown_source_rect, pipedown2_dest_rect, (Vector2){0, 0}, 0, WHITE);
                DrawTexturePro(surface, surface_source_rect, surface_dest_rect, (Vector2){0, 0}, 0, WHITE);
                DrawText(TextFormat("%i", score), screenWidth/2, 30, 60, WHITE);
                DrawFPS (10, 10);
            EndDrawing();
            
            pipeup1_dest_rect.x -= 2;
            pipedown1_dest_rect.x -= 2;
            pipeup2_dest_rect.x -= 2;
            pipedown2_dest_rect.x -= 2;
            
            if ((pipedown1_dest_rect.x+pipe.width)<0)
                pipe_func(&pipeup1_dest_rect, &pipedown1_dest_rect, 550);
            else if ((pipedown2_dest_rect.x+pipe.width)<0)
                pipe_func(&pipeup2_dest_rect, &pipedown2_dest_rect, 550);
            
            if (((flappy_dest_rect.x+flappy.width)-1)==(pipedown1_dest_rect.x+pipe.width/2) ||
                ((flappy_dest_rect.x+flappy.width)-1)==(pipedown2_dest_rect.x+pipe.width/2))
            {
                PlaySound(score_sound);  
                score++;
            }
           
            if (CheckCollisionRecs(flappy_dest_rect, surface_dest_rect) || CheckCollisionRecs(flappy_dest_rect, pipeup1_dest_rect) ||
                CheckCollisionRecs(flappy_dest_rect, pipedown1_dest_rect) || CheckCollisionRecs(flappy_dest_rect, pipeup2_dest_rect) ||
                CheckCollisionRecs(flappy_dest_rect, pipedown2_dest_rect))
            {
                if (highest_score<score)
                    highest_score = score;
                PlaySound(hit_sound);
                game_active = false;
            }
        }
        else
        {
            BeginDrawing();
                ClearBackground(FLAPPY_BLUE);
                DrawText(TextFormat("Score: %i", score), 200, 30, 40, WHITE);
                DrawText(TextFormat("Highest Score: %i", highest_score), 100, 90, 40, WHITE);
                DrawText("Press space to continue...", 50, 250, 35, WHITE);
            EndDrawing();
        }       
    }
    UnloadTexture(background);
    UnloadTexture(flappy);
    UnloadTexture(pipe);
    UnloadTexture(surface);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}