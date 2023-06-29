#include "raylib.h"
#define SIZE 40

const int ScreenWidth = 680, ScreenHeight = 800;
const int left_offset = 0, right_offset = ScreenWidth-280, counter = ScreenHeight/SIZE;
int frame_rate = 2, frame_counter = 0, tetramino_block_no, next_tetramino_block_no, next_block;
bool x_check_left = false, x_check_right = false, game_active = false;

Vector2 block_list[7][5][4] = {
                                //T-block
                                {{{120, -40}, {160, -80}, {200, -40}, {160, -40}}, {{160, -40}, {160, -80}, {200, -40}, {160, 0}},
                                {{120, -40}, {160, -40}, {200, -40}, {160, 0}}, {{120, -40}, {160, -80}, {160, -40}, {160, 0}},
                                {{120, -40}, {160, -80}, {200, -40}, {160, -40}}}, 
                                
                                //O-block
                                {{{160, -40}, {200, -40}, {200, 0}, {160, 0}}, {{160, -40}, {200, -40}, {200, 0}, {160, 0}},
                                {{160, -40}, {200, -40}, {200, 0}, {160, 0}}, {{160, -40}, {200, -40}, {200, 0}, {160, 0}},
                                {{160, -40}, {200, -40}, {200, 0}, {160, 0}}},
                                
                                //Z-block
                                {{{160, -40}, {200, -80}, {200, -40}, {160, 0}}, {{160, -80}, {200, -80}, {240, -40}, {200, -40}},
                                {{160, -40}, {200, -80}, {200, -40}, {160, 0}}, {{160, -80}, {200, -80}, {240, -40}, {200, -40}},
                                {{160, -40}, {200, -80}, {200, -40}, {160, 0}}},
                                
                                //S-block
                                {{{160, -40}, {160, -80}, {200, -40}, {200, 0}}, {{160, -40}, {200, -80}, {240, -80}, {200, -40}},
                                {{160, -40}, {160, -80}, {200, -40}, {200, 0}}, {{160, -40}, {200, -80}, {240, -80}, {200, -40}},
                                {{160, -40}, {160, -80}, {200, -40}, {200, 0}}},
                                
                                //I-block
                                {{{160, -80}, {160, -120}, {160, -40}, {160, 0}}, {{120, -80}, {160, -80}, {240, -80}, {200, -80}},
                                {{160, -80}, {160, -120}, {160, -40}, {160, 0}}, {{120, -80}, {160, -80}, {240, -80}, {200, -80}},
                                {{160, -80}, {160, -120}, {160, -40}, {160, 0}}},
                                
                                //L-block
                                {{{160, -40}, {160, -80}, {200, 0}, {160, 0}}, {{120, -40}, {160, -40}, {200, -40}, {120, 0}},
                                {{120, -80}, {160, -80}, {160, -40}, {160, 0}}, {{120, -40}, {200, -80}, {200, -40}, {160, -40}},
                                {{160, -40}, {160, -80}, {200, 0}, {160, 0}}},
                                
                                //J-block
                                {{{160, 0}, {200, -80}, {200, -40}, {200, 0}}, {{160, 0}, {160, -40}, {240, 0}, {200, 0}},
                                {{160, -40}, {160, -80}, {200, -80}, {160, 0}}, {{120, -40}, {160, -40}, {200, -40}, {200, 0}},
                                {{160, 0}, {200, -80}, {200, -40}, {200, 0}}}
                            };

Color color[] = {VIOLET, YELLOW, MAROON, LIME, SKYBLUE, GOLD, DARKBLUE};

Vector2 tetramino_pos[200], tetramino[5][4];
int prev_color_index[200], block, block_counter = 0, score = 0, highest_score = 0;
Vector2 size = {SIZE, SIZE};

Sound drop, gameover, line, rotate;
Music start_music;

static void tetramino_block(void);
static void update_pos(int x, int y);
static void block_aligned(void);

int main(void)
{
    InitAudioDevice();
    InitWindow(ScreenWidth, ScreenHeight, "Tetris");
    
    Image icon = LoadImage("images/tetris_icon.png");
    SetWindowIcon(icon);
    
    start_music = LoadMusicStream("audio/start_music.wav");
    drop = LoadSound("audio/drop.wav");
    gameover = LoadSound("audio/gameover.wav");
    line = LoadSound("audio/line.wav");
    rotate = LoadSound("audio/rotate.wav");

    next_block = GetRandomValue(0, 4);
    next_tetramino_block_no = GetRandomValue(0, 6);
    tetramino_block();
    
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        UpdateMusicStream(start_music);
        if(game_active)
        {
            PauseMusicStream(start_music);
            if(IsKeyPressed(KEY_UP) && tetramino[block+1][0].x>=left_offset && 
                tetramino[block+1][2].x<right_offset && tetramino[block][3].y<=(ScreenHeight-SIZE*2))
            {
                PlaySound(rotate);
                block++;
                if(block>3)
                    block = 0;
            }
            if(IsKeyDown(KEY_DOWN))
                frame_rate = 20;
            else
                frame_rate = 2;
            
            if(IsKeyDown(KEY_LEFT) && tetramino[block][0].x>left_offset && !x_check_left)
            {
                frame_rate = 15;
                update_pos(-SIZE, 0);
            }
            else if(IsKeyDown(KEY_RIGHT) && tetramino[block][2].x+SIZE<right_offset && !x_check_right)
            {
                frame_rate = 15;
                update_pos(SIZE, 0);
            }
            else
                update_pos(0, SIZE);
            
            BeginDrawing();
                ClearBackground(BLACK);
                for(int i=0; i<4; i++)
                {
                    DrawRectangleV(tetramino[block][i], size, color[tetramino_block_no]);
                    DrawRectangle(block_list[next_tetramino_block_no][next_block][i].x+SIZE*9,
                                  block_list[next_tetramino_block_no][next_block][i].y+SIZE*6, SIZE, SIZE, color[next_tetramino_block_no]);
                }
                
                for(int i=0; i<block_counter; i++)
                    DrawRectangleV(tetramino_pos[i], size, color[prev_color_index[i]]);
                
                for(int i=0; i<counter; i++)
                {
                    DrawLine(0, i*SIZE, ScreenWidth, i*SIZE, BLACK);
                    DrawLine(i*SIZE, 0, i*SIZE, ScreenHeight, BLACK);
                }
                
                DrawRectangleLinesEx((Rectangle){0, 0, right_offset, ScreenHeight}, 5, WHITE);
                DrawText("NEXT SHAPE", right_offset+SIZE, SIZE*2, 30, WHITE);
                DrawText("SCORE", right_offset+SIZE*2, SIZE*15, 30, WHITE);
                DrawText(TextFormat("%d", score), right_offset+SIZE*2, SIZE*16, 50, WHITE);
            EndDrawing();
        }
        else
        {
            if(highest_score<score)
                highest_score = score;
            if(IsKeyPressed(KEY_SPACE))
            {
                frame_counter = block_counter = score = 0;
                x_check_left = x_check_right = false;
                game_active = true;
            }
            BeginDrawing();
                ClearBackground(BLACK);
                if(!highest_score)
                {
                    PlayMusicStream(start_music);
                    DrawText("TETRIS", ScreenWidth/2-160, 300, 80, RED);
                    DrawText("Press space to play...", 140, 400, 40, WHITE);
                }
                else
                {
                    DrawText("GAME OVER", 120, 130, 70, RED);
                    DrawText(TextFormat("Score: %d", score), 120, 330, 50, WHITE);
                    DrawText(TextFormat("Highest Score: %d", highest_score), 120, 400, 50, WHITE);
                    DrawText("Press space to continue...", 80, 530, 40, GREEN);
                }
            EndDrawing();
        }
    }
    UnloadImage(icon);
    UnloadMusicStream(start_music);
    UnloadSound(drop);
    UnloadSound(gameover);
    UnloadSound(line);
    UnloadSound(rotate);
    
    CloseWindow();
    CloseAudioDevice();
    
    return 0;
}

void tetramino_block(void)
{
    x_check_left = x_check_right = false;
    tetramino_block_no = next_tetramino_block_no;
    block = next_block;
    
    next_tetramino_block_no = GetRandomValue(0, 6);
    next_block = GetRandomValue(0, 4);
   
    for(int i=0; i<5; i++)
        for(int j=0; j<4; j++)
            tetramino[i][j] = block_list[tetramino_block_no][i][j];
}

void update_pos(int x, int y)
{
    bool y_check = false;
    frame_counter++;
    for(int i=0; i<block_counter; i++)
    {
        if(y_check)
            break;
        for(int j=0; j<4; j++)
        {
            if(tetramino[block][j].x-SIZE==tetramino_pos[i].x && tetramino[block][j].y==tetramino_pos[i].y)
                x_check_left = true;
            if(tetramino[block][j].x+SIZE==tetramino_pos[i].x && tetramino[block][j].y==tetramino_pos[i].y)
                x_check_right = true;
            if (tetramino[block][j].y+SIZE==tetramino_pos[i].y && tetramino[block][j].x==tetramino_pos[i].x)
            {
                y_check = true;
                if(tetramino[block][1].y<0)
                {
                    game_active = false;
                    PlaySound(gameover);
                }
            }
        }
    }
    if(frame_counter>=(60/frame_rate) && tetramino[block][3].y<=(ScreenHeight-SIZE*2) && !y_check)
    {
        x_check_left = x_check_right = false;
        frame_counter = 0;
        for(int i=0; i<5; i++)
        {
            for(int j=0; j<4; j++)
            {
                tetramino[i][j].x += x;
                tetramino[i][j].y += y;
            }
        }
        if(!x)
            score++;
    }
    else if(tetramino[block][3].y==(ScreenHeight-SIZE) || y_check)
    {
        PlaySound(drop);
        int c = 0;
        for(int i=block_counter; i<(block_counter+4); i++)
        {
            tetramino_pos[i] = tetramino[block][c++];
            prev_color_index[i] = tetramino_block_no;
        }
        block_counter += 4;
        block_aligned();
        tetramino_block();
    }
}

//now this part is probably not that efficient(look at those nested for loops), but I can't come with a better idea...
void block_aligned(void)
{
    int array[20][2] = {0}, c = 0;
    bool line_sound = false;
    
    for(int i=0; i<block_counter; i++)
        for(int j=0; j<20; j++)
        {
            if(tetramino_pos[i].y!=array[j][0] && !array[j][0])
            {
                array[j][0] = tetramino_pos[i].y;
                array[j][1] = 1;
                break;
            }
            else if(tetramino_pos[i].y==array[j][0])
            {
                array[j][1]++;
                break;
            }
        }

    for(int j=19; j>=0; j--)
        if(array[j][1]==10)
        {
            for(int i=0; i<block_counter; i++)
            {
                if(tetramino_pos[i].y==array[j][0])
                {
                    tetramino_pos[i].y = -SIZE;
                    tetramino_pos[i].x = -SIZE;
                }
                else if(tetramino_pos[i].y<array[j][0] && tetramino_pos[i].y>0)
                    tetramino_pos[i].y += SIZE;
            }
            score += SIZE*2;
            line_sound = true;
        }
    
    if(line_sound)
        PlaySound(line);
    
    while(c<block_counter)
    {
        if(tetramino_pos[c].x<0)
        {
            for(int i=c+1; i<block_counter; i++)
            {
                tetramino_pos[i-1] = tetramino_pos[i];
                prev_color_index[i-1] = prev_color_index[i];
            }
            block_counter--;
        }
        else
            c++;
    }
}
