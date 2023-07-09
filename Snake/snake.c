#include "raylib.h"

#define SIZE 40
#define SNAKE_SIZE 330
#define SAP_GREEN (Color){149, 232, 78, 255}

const int ScreenWidth = 960, ScreenHeight = 680, counter = ScreenWidth/SIZE;
const int left_offset = SIZE, right_offset = ScreenWidth-SIZE*2, upper_offset = SIZE, lower_offset = ScreenHeight-SIZE*2;
bool game_active = false;

int x_mov = 0, y_mov = 0, frames_counter = 0, frame_rate = 8, snake_length = 1;
int highest_score = 0;
Vector2 snake_array[SNAKE_SIZE], size = {SIZE, SIZE}, food_pos = {0};

void movement(int x_steps, int y_steps);
void initSnakeAndFoodPos(void);

int main(void)
{    
    InitWindow(ScreenWidth, ScreenHeight, "Snake");
    
    Image icon = LoadImage("images/snake_icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
    
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        if(game_active)
        {
            frames_counter++;
            if(frames_counter >= 60/frame_rate)
            {
                bool regenerate_food = false;
                frames_counter = 0;
                
                if(snake_array[0].x==food_pos.x && snake_array[0].y==food_pos.y  && snake_length!=SNAKE_SIZE)
                {
                    snake_length++;
                    snake_array[snake_length].x = -SIZE;
                    snake_array[snake_length].y = -SIZE;
                    regenerate_food = true;
                }
                
                for(int i=snake_length-1; i>=1; i--)
                {
                    snake_array[i].x = snake_array[i-1].x;
                    snake_array[i].y = snake_array[i-1].y;
                }
                snake_array[0].x += x_mov;
                snake_array[0].y += y_mov;
                
                while(regenerate_food)
                {   
                    regenerate_food = false;
                    food_pos.x = GetRandomValue(left_offset/SIZE, right_offset/SIZE)*SIZE;
                    food_pos.y = GetRandomValue(upper_offset/SIZE, lower_offset/SIZE)*SIZE;
                    
                    for(int i=0; i<snake_length; i++)
                        if(food_pos.x==snake_array[i].x && food_pos.y==snake_array[i].y)
                        {
                            regenerate_food = true;
                            break;
                        }
                }
            }
            
            for(int i=1; i<snake_length; i++)
                if(snake_array[i].x==snake_array[0].x && snake_array[i].y==snake_array[0].y)
                {
                    game_active = false;
                    break;
                }
            
            if(snake_array[0].x<left_offset || snake_array[0].x>right_offset || snake_array[0].y<upper_offset
               || snake_array[0].y>lower_offset)
               game_active = false;
            
            if(IsKeyPressed(KEY_DOWN) && !y_mov)
                movement(0, SIZE);
            else if(IsKeyPressed(KEY_UP) && !y_mov)
                movement(0, -SIZE);
            else if(IsKeyPressed(KEY_RIGHT) && !x_mov)
                movement(SIZE, 0);
            else if(IsKeyPressed(KEY_LEFT) && !x_mov)
                movement(-SIZE, 0);
            
            BeginDrawing();
                ClearBackground(SAP_GREEN);
                
                for(int i=0; i<snake_length; i++)
                    DrawRectangleV(snake_array[i], size, (i)? DARKGRAY:BLACK);
                DrawRectangleV(food_pos, size, RED);
                
                for(int i=1; i<counter; i++)
                {
                    if(i*SIZE>upper_offset && i*SIZE<=lower_offset)
                        DrawLine(left_offset, SIZE*i, right_offset+SIZE, SIZE*i, BLACK);
                    if(i*SIZE>left_offset && i*SIZE<=right_offset)
                        DrawLine(SIZE*i, upper_offset, SIZE*i, lower_offset+SIZE, BLACK);
                }
                DrawRectangleLinesEx((Rectangle){left_offset, upper_offset, right_offset, lower_offset}, 3, BLACK);
                DrawText(TextFormat("Score: %d", snake_length-1), left_offset, 5, 30, BLACK);
            EndDrawing();
        }
        else
        {
            if(snake_length-1>highest_score)
                highest_score = snake_length-1;
            if(IsKeyPressed(KEY_SPACE))
            {
                x_mov = y_mov = 0;
                snake_length = 1;
                initSnakeAndFoodPos();
                game_active = true;
            }
            
            BeginDrawing();
                ClearBackground(SAP_GREEN);
                if(!food_pos.x)
                {
                    DrawText("SNAKE GAME", 220, 220, 80, RED);
                    DrawText("Press space to play...", 270, 320, 40, VIOLET);
                }
                else
                {
                    if(snake_length!=SNAKE_SIZE)
                        DrawText("GAME OVER!", 280, 150, 70, RED);
                    else
                        DrawText("YOU WON!", 280, 150, 70, RED);
                    DrawText(TextFormat("Score: %d", snake_length-1), 280, 300, 50, BLACK);
                    DrawText(TextFormat("Highest Score: %d", highest_score), 280, 370, 50, BLACK);
                    DrawText("Press space to continue...", 220, 500, 40, VIOLET);
                }
            EndDrawing();
        }
    }
    CloseWindow();
    
    return 0;
}

void movement(int x_steps, int y_steps)
{   
    x_mov = x_steps;
    y_mov = y_steps;
}

void initSnakeAndFoodPos(void)
{
    snake_array[0].x = GetRandomValue(left_offset/SIZE, 11)*SIZE;
    snake_array[0].y = GetRandomValue(upper_offset/SIZE, lower_offset/SIZE)*SIZE;
    
    food_pos.x = GetRandomValue(13, right_offset/SIZE)*SIZE;
    food_pos.y = GetRandomValue(upper_offset/SIZE, lower_offset/SIZE)*SIZE;
}
