#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

struct Video{
    int width, heigth;
    bool running;
    int fps;
    SDL_Window *window;
    SDL_Renderer *renderer;
};

struct Map{
    bool grid[10][10];
};

struct Player{
    double x;
    double y;
    double dx;
    double dy;
    double alpha;
    SDL_Rect box;
};

//int check_colision_player(struct Player player, int x, int y, int w, int h){
//    if (player.x + player.dx == x && player.y + player.dy == y){
//        return 1;
//    }
//    else if (player.x + player.dx == x){
//        return 2;
//    }
//    else if (player.y + player.dy == y){
//        return 3;
//    }
//    return 0;
//}

float distance(double x1,double y1,double x2,double y2){
    return sqrt((x2 - x1)*(x2 - x1) +(y2 - y1)*(y2 - y1));
}

int player_controller(struct Player *player, const Uint8* keyboard_state_array){
    if (keyboard_state_array[SDL_SCANCODE_W]) {
        player -> x += player -> dx;
        player -> y += player -> dy;
        //printf("%i \n", player -> y);
    }
    if (keyboard_state_array[SDL_SCANCODE_S]) {
        player ->x -= player -> dx;
        player ->y -= player -> dy;
        //printf("%i \n, player -> y");
    }
    if (keyboard_state_array[SDL_SCANCODE_A]) {
        player -> alpha -= 0.1;
        if (player -> alpha < 0){ player -> alpha += 6.28;}
        player -> dx = cos(player -> alpha) * 5;
        player -> dy = sin(player -> alpha) * 5;
    }
    if (keyboard_state_array[SDL_SCANCODE_D]) {
        player -> alpha += 0.1;
        if (player -> alpha > 6.28){ player -> alpha -= 6.28;}
        player -> dx = cos(player -> alpha) * 5;
        player -> dy = sin(player -> alpha) * 5;
    }
}

//desenha e calcula o FOV
int draw_fov(int resolution, double fov, struct Player player, struct Video video, bool map[10][10]){
    int map_x, map_y, dof;
    double internal_ang, teta_0, teta, rx, ry, yo, xo, dist_vert, dist_hor, rx_vert, rx_hor, ry_vert, ry_hor, dist, dist_corrigida;
    //double distances[resolution] = {-1};
    internal_ang = fov/resolution;
    teta_0 = player.alpha - internal_ang*resolution/2;
    for (int i = 0; i <= resolution; i += 1){
        dof = 0;
        ////checagem para o horizontal
        teta = teta_0 + i*internal_ang; if (teta < 0){ teta += 6.28;} else if (teta > 6.28){ teta -= 6.28; }
        double teta_a_tan = -1/tan(teta);
        //angulo para cima
        if (teta > 3.1416) { 
            ry =(((int)(player.y + 5)>>6)<<6) -0.0001; 
            rx=(player.y + 5 - ry)*teta_a_tan+player.x+5; 
            yo=-64; 
            xo=-yo*teta_a_tan; 
        }
        //angulo para baixo
        if (teta < 3.1416) { 
            ry =(((int)(player.y + 5)>>6)<<6) +64; 
            rx=(player.y + 5 - ry)*teta_a_tan+player.x+5; 
            yo= 64; 
            xo=-yo*teta_a_tan; 
        }
        //olhando pro lado
        if (teta == 0 || teta == 3.1416) {
            rx = player.x + 5;
            ry = player.y + 5;
            yo=-64;
            xo=-yo*teta_a_tan;
            dof = 8;
        }

        while (dof < 8){
            map_x = (int)rx >> 6;
            map_y = (int)ry >> 6;
            if ((map_x < 10 && map_y < 10) && map[map_y][map_x] == 0){
                rx += xo;
                ry += yo;
                rx_hor = rx;
                ry_hor = ry;
                dof++;
            }
            else{
                dof = 8;
                rx_hor = rx;
                ry_hor = ry;
            }
        }
        dist_hor = distance(player.x+5, player.y+5, rx_hor,ry_hor);

        //angulo para esquerda
        dof = 0;
        teta = teta_0 + i*internal_ang; if (teta < 0){ teta += 6.28;} else if (teta > 6.28){ teta -= 6.28; }
        double teta_tan = -tan(teta);
        if (teta > 3.1416/2 && teta < 3*3.1416/2) { 
            rx=(((int)(player.x + 5)>>6)<<6) -0.0001; 
            ry=(player.x + 5 - rx)*teta_tan+player.y+5; 
            xo=-64; 
            yo=-xo*teta_tan; 
        }
        //angulo para direita
        if (teta < 3.1416/2 || teta > 3*3.1416/2) { 
            rx =(((int)(player.x + 5)>>6)<<6) +64; 
            ry=(player.x + 5 - rx)*teta_tan+player.y+ 5; 
            xo= 64; 
            yo=-xo*teta_tan;
        }
        //olhando pro cima/baixo
        if (teta == 3.1416/2 || teta == 3*3.1416/2) {
            rx = player.x + 5;
            ry = player.y + 5;
            dof = 8;
        }
        while (dof < 8){
            map_x = (int)rx >> 6;
            map_y = (int)ry >> 6;
            if ((map_x < 10 && map_y < 10) && map[map_y][map_x] == 0){
                rx += xo;
                ry += yo;
                rx_vert = rx;
                ry_vert = ry;
                dof++;
            }
            else{
                dof = 8;
                rx_vert = rx;
                ry_vert = ry;
            }
        }
        dist_vert = distance(player.x+5, player.y+5, rx,ry);
        if (dist_vert < dist_hor){
            //SDL_RenderDrawLineF(video.renderer, player.x + 5, player.y + 5, rx_vert, ry_vert);
            dist = dist_vert;
        }
        else {
            //SDL_RenderDrawLineF(video.renderer, player.x + 5, player.y + 5, rx_hor, ry_hor);
            dist = dist_hor;
        }
        //desenha paredes
        SDL_Rect wall = {0};
        double ca = player.alpha - teta;
        if (ca > 2*3.1416){ ca -= 6.28;}
        if (ca < 0) { ca+=6.28; }
        dist_corrigida = dist*cos(ca);
        wall.x = i*video.width/resolution;
        wall.h = (video.heigth*video.heigth)/(10*(dist + dist_corrigida)/2);
        wall.y = video.heigth/2 - wall.h/2;
        wall.w = video.width/resolution;
        if (dist == dist_hor){
            SDL_SetRenderDrawColor(video.renderer, 0,0,255,255);
        }
        else{
            SDL_SetRenderDrawColor(video.renderer, 0,0,200,255);
        }
        SDL_RenderDrawRect(video.renderer, &wall);
        SDL_RenderFillRects(video.renderer, &wall, 1);
        SDL_SetRenderDrawColor(video.renderer, 200,200,200,255);
    }
    return 0;
}

int draw_player(struct Player player, struct Video video, bool map[10][10]){
    player.box.x = player.x;
    player.box.y = player.y;
    player.box.w = 10;
    player.box.h = 10;
    //SDL_RenderDrawRect(video.renderer, &player.box);
    //SDL_RenderFillRects(video.renderer, &player.box, 1);
    //SDL_RenderDrawLineF(video.renderer,
    //                    player.x + 5, player.y + 5, player.x + 5 + player.dx*5, player.y + 5 + player.dy*5);
    draw_fov(video.width, 3.1416/2, player, video, map);
    return 0;
}


int keep_fps(int starttime, int endtime, int fps){
    int delta = endtime - starttime;
    if (delta <= 1000/fps){
        SDL_Delay(1000/fps - delta);
    }
    return 0;
}

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO);


    //inicialisa as condicoes para a janela
    struct Video video = {0};
    video.width = 1280;
    video.heigth = 720;
    video.window = NULL;
    video.renderer = NULL;
    video.fps = 60;

    //pega o input
    const Uint8* keyboard_state_array = SDL_GetKeyboardState(NULL);


    //inicialisa o struct jogador
    struct Player player = {0};
    player.x = video.heigth/2 - 10;
    player.y = video.width/2 - 10;
    player.dx = 0;
    player.dy = 1;
    player.alpha = 0;



    //struct Map map = {};

    bool grid[10][10] = {
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,0,0,0,0,0,1},
    {1,0,1,1,1,0,0,0,0,1},
    {1,0,1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1}
    };


    SDL_Rect box;
    box.h = video.heigth/10;
    box.w = video.width/10;

    SDL_CreateWindowAndRenderer(video.width, video.heigth, SDL_WINDOW_OPENGL, &video.window, &video.renderer);

    SDL_Event event;
    video.running = true;
   
    int endtime = 1000;
    int starttime = 0;
    int deltatime;

    SDL_Rect floor;
    floor.x = 0;
    floor.y = video.heigth/2;
    floor.h = floor.y;
    floor.w = video.width;
    while(video.running){
        keep_fps(starttime, endtime, video.fps);
        starttime = SDL_GetTicks();
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                {
                    video.running = false;
                } break;
                default: {} break;
            }
        }

        //desenha o mapa
        int colisao = 0;
        SDL_SetRenderDrawColor(video.renderer, 100,100,100,255);
        SDL_RenderDrawRect(video.renderer, &floor);
        SDL_RenderFillRects(video.renderer, &floor, 1);
        for (int i = 0; i < 10; i++){
            for (int j = 0; j < 10; j++){
                if (grid[i][j] == 1){
                    SDL_SetRenderDrawColor(video.renderer,100,100,100,255);
                    box.x = box.w * j;
                    box.y = box.h * i;
                    //SDL_RenderDrawRect(video.renderer, &box);
                    //SDL_RenderFillRects(video.renderer, &box, 1);
                    //SDL_SetRenderDrawColor(video.renderer,0,0,0,0);
                }
            }
        }
        //recebe input e lida com ele
        player_controller(&player, keyboard_state_array);

        //desenha o jogador
        SDL_SetRenderDrawColor(video.renderer,200,200,200,255);
        draw_player(player, video, grid);
        SDL_SetRenderDrawColor(video.renderer,130,130,130,255);


        //
        SDL_RenderPresent(video.renderer);
        SDL_RenderClear(video.renderer);
        endtime = SDL_GetTicks();
        //printf("%i \n", deltatime);
        //if (1000/60 > deltatime){
        //    SDL_Delay(1000/60 - deltatime);
        //}  
    }

    SDL_DestroyRenderer(video.renderer);
    SDL_DestroyWindow(video.window);
    SDL_Quit();

    return 0;
}