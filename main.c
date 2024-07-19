#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

#define Width 800
#define Height 600

#define CENTER(WH, a) (WH / 2) - (a / 2) 
#define MIN(a, b) (a < b) ? a : b

typedef enum { MENU = 0, GAME, CONTINUE, QUIT, DEATH } GameScreen;

typedef struct {
    int vel_o;
    int score;
    int index_o;
    int min;
} Helpers;

float get_y() {
    int chance = GetRandomValue(0, 10);
    return (chance > 6) ? 500 : 540;
}

void init(Rectangle *p, Vector2 *vel, Rectangle *o, Helpers *h) {
    p->x = 0;
    p->y = 550;
    p->height = p->width = 50;
    vel->x = vel->y = 0;
    h->vel_o = 320;
    h->score = 0;
    h->index_o = 4;
    h->min = 400;
    o[0].x = GetScreenWidth() + 600;
    o[0].y = get_y();
    for(int i = 0; i < 5; i++) {
        float tmp = o[i - 1].x;
        o[i].x = GetRandomValue(tmp + 400, tmp + 500);
        o[i].y = get_y();
        o[i].width = GetRandomValue(30, 50);
        o[i].height = 60;
    }
}



int main() {
    InitWindow(Width, Height, "dindin");
    Rectangle player;
    Vector2 p_vel;
    Helpers h;
    Rectangle obstacles[5];

    init(&player, &p_vel, obstacles, &h);
    
    bool pause = 0;
    GameScreen state = MENU;
    Vector2 mouse_pos = {0, 0};
    Vector2 mouse_pos_prev = {0, 0};

    SetTargetFPS(60);
    SetExitKey(0);
    bool run = 1;
    int ind_menu = 0;
    bool mouse_cntrl = true;
    int frame_counter = 0;

    while (!WindowShouldClose() && run) {
        BeginDrawing();
        switch (state) {
            case MENU: {
                ClearBackground(RAYWHITE);
                Rectangle texts[3];
                char *text[3] = {"New Game", "Continue", "Quit"};
                int ys = 200;
                for(int i = 0; i < 3; i++){
                    Color color = BLACK;
                    int text_size = MeasureText(text[i], 40);
                    if(i == 1 && !pause) 
                        color = LIGHTGRAY;
                    DrawText(text[i], CENTER(Width, text_size), ys, 40, color);
                    texts[i].x = CENTER(Width, text_size) - 10;
                    texts[i].y = ys - 10;
                    texts[i].width = text_size + 20;
                    texts[i].height = 60;
                    ys += 70; 
                }
                mouse_pos = GetMousePosition();
                for(int i = 0; i < 3 && mouse_cntrl; i++){
                    if(!pause && i == 1) continue;
                    if(CheckCollisionPointRec(mouse_pos, texts[i])){
                        ind_menu = i;
                        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            state = i + 1;
                            break;
                        }
                    }
                }
                if(!CheckCollisionCircles(mouse_pos, .5, mouse_pos_prev, .5))
                    mouse_cntrl = true;

                mouse_pos_prev.x = mouse_pos.x;
                mouse_pos_prev.y = mouse_pos.y;

                if(IsKeyPressed(KEY_DOWN)){
                    mouse_cntrl = false;
                    ind_menu++;
                    if(ind_menu == 1 && !pause) ind_menu++;
                    if(ind_menu > 2) ind_menu = 0;
                }
                if(IsKeyPressed(KEY_UP)){
                    mouse_cntrl = false;
                    ind_menu--;
                    if(ind_menu == 1 && !pause) ind_menu--;
                    if(ind_menu < 0) ind_menu = 2;
                }
                if(IsKeyPressed(KEY_ENTER)) 
                    state = ind_menu + 1;

                DrawRectangleRec(texts[ind_menu], (Color){200, 200, 200, 100});
                
            } break;
            case GAME: {
                init(&player, &p_vel, obstacles, &h); 
                state = CONTINUE; 
            } break;
            case CONTINUE: {
                ClearBackground(RAYWHITE);
                if(player.x < (double) Width / 5) 
                    p_vel.x = 400;
                else p_vel.x = 0;

                p_vel.y += 2000 * GetFrameTime();

                if(IsKeyPressed(KEY_SPACE) && player.y + 1 > Height - player.height) 
                    p_vel.y = -600;
                else if(IsKeyDown(KEY_DOWN) && player.y >= 550){
                    player.y = 570;
                    player.height = 30;
                } else if(!IsKeyDown(KEY_DOWN) && player.y >= 550) {
                    player.y = 550;
                    player.height = 50;
                }
                if(IsKeyPressed(KEY_ESCAPE)){
                    state = MENU;
                    ind_menu = 0;
                    pause = true;
                }

                player.x += p_vel.x * GetFrameTime();
                player.y += p_vel.y * GetFrameTime();

                for(int i = 0; i < 5; i++){
          
                    obstacles[i].x -= h.vel_o * GetFrameTime();
                    if(obstacles[i].x + obstacles[i].width < 0){
                        h.score += 10;
                        int tmp = GetRandomValue(h.min, (h.min + 100));
                        obstacles[i].x = obstacles[h.index_o].x + tmp;
                        obstacles[i].y = get_y();
                        h.index_o++;
                        if(h.index_o > 4) h.index_o = 0;
                        if(!(h.score%100) && h.score > 0){
                            h.vel_o += 20;
                            h.min += 5;
                        }

                    }
                    
                }

                player.y = MIN(player.y, (Height - player.height));
                
                char score_buff[100];
                snprintf(score_buff, 100, "Score %d", h.score);
                int sc_size = MeasureText(score_buff, 20);

                DrawText(score_buff, Width - sc_size - 10, 0, 20, BLACK);
                DrawRectangleRec(player, GREEN);
                for(int i = 0; i < 5; i++){
                    if(CheckCollisionRecs(obstacles[i], player)) {
                        state = DEATH;
                        pause = false;
                        ind_menu = 0;
                    }
                    DrawRectangleRec(obstacles[i], BLACK);
                }
            } break;
            case DEATH: {
                int y = CENTER(Height, 100);
                DrawRectangle(0, y, Width, 80, BLACK);
                int text_size = MeasureText("YOU DIED", 40);
                DrawText("YOU DIED", CENTER(Width, text_size), y + 20, 40, RED);
                frame_counter++;
                if(frame_counter > 90) {
                    frame_counter = 0;
                    state = MENU;
                }
            } break;
            case QUIT: {
                run = 0;
            } break;
            default: break;
        }
        DrawFPS(10, 10); 
        EndDrawing();
    }


    CloseWindow();
    return 0;
}
