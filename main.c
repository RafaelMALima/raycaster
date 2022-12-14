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

int check_colision_player(struct Player *player, SDL_Rect box){
   //colisao frontal
    if ((player -> x + player -> dx > box.x && player -> x + player -> dx < box.x + box.w ) // checa no eixo x
    && (player -> y + player -> dy > box.y && player -> y + player -> dy < box.y + box.h )) //checa o eixo y
    {
        return 1; // 1 = colisao frontal
    }
    if ((player -> x - player -> dx > box.x && player -> x - player -> dx < box.x + box.w ) // checa no eixo x
    && (player -> y - player -> dy > box.y && player -> y - player -> dy < box.y + box.h)) //checa o eixo y
    { 
        return 2; // 2 = colisao traseira
    }
    return 0;
}

float distance(double x1,double y1,double x2,double y2){
    return sqrt((x2 - x1)*(x2 - x1) +(y2 - y1)*(y2 - y1));
}

int player_controller(struct Player *player, const Uint8* keyboard_state_array,int map[10][10], struct Video video){
    int  coordinate_x, coordinate_y;
    coordinate_x = (int)player -> x >> 6;
    coordinate_y = (int)player -> y >> 6;
    int colision = 0;
    int b_colision;
    for (int i = coordinate_x - 1; i <= coordinate_x + 1; i++){
        for (int j = coordinate_y - 1; j <= coordinate_y + 1; j++){
            //printf("%i, %i \n",i,j);
            if (map[j][i] == 1){
                SDL_Rect box;
                box.x = (i << 6);
                box.y = (j << 6);
                box.w = 64;
                box.h = 64;
                //printf("%i, %i, %f, %f\n",box.x , box.y, player -> x, player -> y);
                b_colision = check_colision_player(player, box);
                if (colision == 0){
                    //printf("%i, %i, %i, %i, %f, %f \n",box.x, box.x + box.w, box.y, box.y + box.h, player -> x + player -> dx, player -> y + player -> dy);
                    colision = b_colision;
                }
                printf("%i \n",colision);
            }
        }
    }
    if (keyboard_state_array[SDL_SCANCODE_W] && colision != 1) {
        player -> x += player -> dx;
        player -> y += player -> dy;
    }
    if (keyboard_state_array[SDL_SCANCODE_S] && colision != 2) {
        player ->x -= player -> dx;
        player ->y -= player -> dy;
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
int draw_fov(int resolution, double fov, struct Player player, struct Video video, int map[10][10]){
    int map_x, map_y, dof;
    double internal_ang, teta_0, teta, rx, ry, yo, xo, dist_vert, dist_hor, rx_vert, rx_hor, ry_vert, ry_hor, dist, dist_corrigida, delta_teta, teta_a_tan;
    internal_ang = fov/resolution;
    teta_0 = player.alpha - internal_ang*resolution/2;
    for (int i = 0; i <= resolution; i += 1){
        dof = 0;
        //checagem para o horizontal
        teta = teta_0 + i*internal_ang; if (teta < 0){ teta += 6.28;} else if (teta > 6.28){ teta -= 6.28; }
        teta_a_tan = -1/tan(teta);
    if (teta_a_tan < -1000){ teta_a_tan = -1000; }
        //angulo para baixo
        if (teta > 3.141) { 
            ry =(((int)(player.y + 5)>>6)<<6) -0.0001;
            rx=(player.y + 5 - ry)*teta_a_tan+player.x+5;
            yo=-64;
            xo=-yo*teta_a_tan;
        }
        //angulo para cima
        if (teta < 3.141) { 
            ry =(((int)(player.y + 5)>>6)<<6) +64; 
            rx=(player.y + 5 - ry)*teta_a_tan+player.x+5; 
            yo= 64; 
            xo= -yo*teta_a_tan;
        }
        //olhando pro lado
        if (teta == 0 || teta == 3.1416) {
            rx = player.x + 5;
            ry = player.y + 5;
            dof = 8;
        }
        while (dof < 8){
            map_x = (int)rx >> 6;
            map_y = (int)ry >> 6;
            if (map_x < 10 && map_y < 10 && map[map_y][map_x] == 0){
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
        double teta_tan = -tan(teta);
        if (teta_tan > 1000){ teta_tan = 1000; }
        if (teta_tan < -1000){ teta_tan = -1000; }
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
        SDL_Rect wall;
        delta_teta = player.alpha - teta;
        if (delta_teta > 2*3.1416){ delta_teta -= 6.28;}
        if (delta_teta < 0) { delta_teta+=6.28; }
        dist_corrigida = dist*cos(delta_teta);
        wall.x = i*video.width/resolution;
        wall.h = (video.heigth*video.heigth)/(10*(dist + 3*dist_corrigida)/4);
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

int draw_player(struct Player player, struct Video video, int map[10][10]){
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
    player.x = 310;
    player.y = 310;
    player.dx = 5;
    player.dy = 0;
    player.alpha = 0;


    int grid[10][10] = {
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,1,0,0,0,0,1},
    {1,0,1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1}
    };


    SDL_Rect box;
    box.h = 64;
    box.w = 64;

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
        player_controller(&player, keyboard_state_array, grid, video);

        //desenha o jogador
        SDL_SetRenderDrawColor(video.renderer,200,200,200,255);
        draw_player(player, video, grid);
        SDL_SetRenderDrawColor(video.renderer,130,130,130,255);


        //
        SDL_RenderPresent(video.renderer);
        SDL_RenderClear(video.renderer);
        endtime = SDL_GetTicks();
    }

    SDL_DestroyRenderer(video.renderer);
    SDL_DestroyWindow(video.window);
    SDL_Quit();

    return 0;
}