//sounds are taken from <http://web.archive.org/web/20180309023632/http://cs.au.dk/~dsound/DigitalAudio.dir/Greenfoot/Pong.dir/Pong.html>
#include "raylib.h"
#include <stdlib.h>

const int ScreenWidth = 1000;
const int ScreenHeight = 800;

Sound pad_hit, wall_hit, score;

int ball_speed_x = 4, ball_speed_y = 2, left_pad_speed = 7;
int left_score = 0, right_score = 10;
bool game_active = true;
char game_over_text[20];

void control_pad(Rectangle *pad);
void ball_movement(Rectangle *ball, Rectangle *right_pad, Rectangle *left_pad);
void reset_ball(Rectangle *ball);

int main(void)
{   
    InitAudioDevice();
    InitWindow(ScreenWidth, ScreenHeight, "Pong");
    
    pad_hit = LoadSound("audio/pad_hit.wav");
    wall_hit = LoadSound("audio/wall_hit.wav");
    score = LoadSound("audio/score.wav");
 
    Rectangle left_pad = {40, ScreenHeight/2, 20, 100};
    Rectangle right_pad = {ScreenWidth-60, ScreenHeight/2, 20, 100};
    Rectangle ball = {ScreenWidth/2, ScreenHeight/2, 20, 20};
    
    Texture2D net = LoadTexture("images/mid_border.png");
    
    Image icon = LoadImage("images/pong_icon.png");
    SetWindowIcon(icon);
    
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_SPACE) && !game_active)
        {
            left_score = right_score = 0;
            ball_speed_x = 4, ball_speed_y = 2;
            game_active = true;
        }
        if(game_active)
        {
            if(IsKeyDown(KEY_DOWN))
                right_pad.y += 7;
            else if(IsKeyDown(KEY_UP))
                right_pad.y -= 7;
            
            //left_pad AI
            if(ball.x<=ScreenWidth/2)
            {
                if(left_pad.y>ball.y)
                    left_pad.y -= left_pad_speed;
                if(left_pad.y<ball.y)
                    left_pad.y += left_pad_speed;
            }
            
            if(left_score==10)
            {
                TextCopy(game_over_text, "Computer Won!");
                game_active = false;
            }
            else if(right_score==10)
            {
                TextCopy(game_over_text, "Player Won!");
                game_active = false;
            }
            
            ball_movement(&ball, &right_pad, &left_pad);
            
            control_pad(&right_pad);
            control_pad(&left_pad);
            
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTexture(net, ScreenWidth/2-50, 0, WHITE);
                DrawRectangleRec(left_pad, WHITE);
                DrawRectangleRec(right_pad, WHITE);
                DrawRectangleRec(ball, WHITE);
                DrawText(TextFormat("%i", left_score), ScreenWidth/4, 40, 80, WHITE);
                DrawText(TextFormat("%i", right_score), (ScreenWidth - ScreenWidth/4-50), 40, 80, WHITE);
            EndDrawing();
        }
        else
        {
            BeginDrawing();
                ClearBackground(BLACK);
                DrawText(game_over_text, 240, 300, 80, WHITE);
                DrawText("Press space to continue...", 270, 400, 35, WHITE);
            EndDrawing();
        }
    }
    UnloadTexture(net);
    UnloadImage(icon);
    UnloadSound(pad_hit);
    UnloadSound(wall_hit);
    UnloadSound(score);
    
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}

void control_pad(Rectangle *pad)
{
    if (pad->y <= 0)
        pad->y = 0;
    else if((pad->y + pad->height) >= ScreenHeight)
        pad->y = ScreenHeight - pad->height;
}

void ball_movement(Rectangle *ball, Rectangle *right_pad, Rectangle *left_pad)
{   
    ball->x += ball_speed_x;
    ball->y += ball_speed_y;
    
    if(ball->y <= 0 || (ball->y+ball->height) >= ScreenHeight)
    {
        PlaySound(wall_hit);
        ball_speed_y *= -1;
    }
    
    if(ball->x < 0)
    {
        right_score++;
        reset_ball(ball);
    }
    else if((ball->x+ball->width) >= ScreenWidth)
    {
        left_score++;
        reset_ball(ball);
    }
    
    if(CheckCollisionRecs(*ball, *right_pad))
    {
        PlaySound(pad_hit);
        if(abs(ball_speed_x)<7)
            ball_speed_x = ball_speed_y = 7;
        if(abs(ball->x+ball->width-right_pad->x)<10)
            ball_speed_x *= -1;
        else if(abs(ball->y+ball->height-right_pad->y)<10 && ball_speed_y>0)
            ball_speed_y *= -1;
        else if(abs(ball->y-(right_pad->x+right_pad->height))<10 && ball_speed_y<0)
            ball_speed_y *= -1;
    }
    else if(CheckCollisionRecs(*ball, *left_pad))
    {
        PlaySound(pad_hit);
        if(abs(ball_speed_x)<7)
            ball_speed_x = ball_speed_y = 7;
        if(abs(ball->x-(left_pad->x+left_pad->width)<10))
            ball_speed_x *= -1;
        else if(abs(ball->y+ball->height-left_pad->y)<10 && ball_speed_y>0)
            ball_speed_y *= -1;
        else if(abs(ball->y-(left_pad->x+left_pad->height))<10 && ball_speed_y<0)
            ball_speed_y *= -1;
    }
}

void reset_ball(Rectangle *ball)
{
    PlaySound(score);
    ball->x = ScreenWidth/2;
    ball->y = ScreenHeight/2;
    ball_speed_x = 4;
    ball_speed_y = GetRandomValue(-3, 3);
}
