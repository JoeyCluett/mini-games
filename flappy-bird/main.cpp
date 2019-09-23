#include <iostream>
#include <list>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include "sdl_core.h"

using namespace std;

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    auto* s = SDL_SetVideoMode(350, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);

    struct {

        bool running = true;
        int  velocity = 0;
        int  gravity = 1;
        int  bird_y = 80;

        const int half_height = 10;
        const int bird_x = 75;
        const int space_cols_hor   = 200;
        const int space_cols_ver   = 150; // 100; // 35; //100;
        const int cols_width = 50;
        const int col_speed = 4;
     
        list<pair<int, int>> columns;

    } vars;

    // need the first column
    vars.columns.push_back({350, 200});
    srand(time(NULL));

    auto render_bird = [&vars](SDL_Surface* s) -> void {

        SDL_Rect r;
        r.w = 2 * vars.half_height;
        r.h = 2 * vars.half_height;
        r.y = vars.bird_y - vars.half_height;
        r.x = vars.bird_x;

        SDL_FillRect(s, &r, SDL_MapRGB(s->format, 255, 0, 0));

    };

    auto render_base = [&vars](SDL_Surface* s) -> void {

        SDL_Rect r;
        r.w = 350;
        r.h = 75;
        r.y = 600 - 150;
        r.x = 0;
        SDL_FillRect(s, &r, SDL_MapRGB(s->format, 0, 75, 0)); // brown

        r.y = 600 - 75;
        SDL_FillRect(s, &r, SDL_MapRGB(s->format, 165, 42, 42)); // brown

    };

    auto render_cols = [&vars](SDL_Surface* s) -> void {
        for(auto col : vars.columns) {

            SDL_Rect r;
            r.x = col.first;
            r.y = col.second + 10;
            r.h = 600 - r.y;
            r.w = vars.cols_width;

            SDL_FillRect(s, &r, SDL_MapRGB(s->format, 0, 0, 255));

            r.x = col.first;
            r.w = vars.cols_width;
            r.h = r.y - vars.space_cols_ver;
            r.y = 0;

            SDL_FillRect(s, &r, SDL_MapRGB(s->format, 0, 255, 0));

        }
    };

    constexpr struct {
        const int starting = 0;
        const int playing  = 1;
        const int endgame  = 2;    
    } STATE;
    int currentstate = STATE.starting;

    // pressing space starts the game
    sdl_event_map_t event_starting_map = {
        {
            SDL_KEYDOWN,
            [&vars, &currentstate](void* ptr) {
                auto* key_event = reinterpret_cast<SDL_KeyboardEvent*>(ptr);
                auto sym = key_event->keysym.sym;
                
                if(sym == SDLK_ESCAPE)
                    vars.running = false;
                else if(sym == SDLK_SPACE)
                    currentstate = STATE.playing;
            }
        }
    };

    // capture quit condition and keystrokes
    sdl_event_map_t event_play_map = {
        {
            SDL_KEYDOWN,
            [&vars](void* ptr) {
                auto* key_event = reinterpret_cast<SDL_KeyboardEvent*>(ptr);
                auto sym = key_event->keysym.sym;
                
                if(sym == SDLK_ESCAPE)
                    vars.running = false;
                else if(sym == SDLK_SPACE)
                    vars.velocity = -10; //-13; // was -20
            }
        }
    };

    // when the player loses, wait for the escape key to be pressed
    sdl_event_map_t event_endgame_map = {
        {
            SDL_KEYDOWN,
            [&vars](void* ptr) {
                auto* key_event = reinterpret_cast<SDL_KeyboardEvent*>(ptr);
                auto sym = key_event->keysym.sym;                
                if(sym == SDLK_ESCAPE)
                    vars.running = false;
            }
        }
    };

    auto player_collides = [&vars](void) -> bool {

        auto rects_collide = [](SDL_Rect& a, SDL_Rect& b) -> bool {
            
            auto len_x = (a.x+(a.w/2)) - (b.x+(b.w/2));
            auto len_y = (a.y+(a.h/2)) - (b.y+(b.h/2));
            if(len_x < 0) len_x *= -1;
            if(len_y < 0) len_y *= -1;

            auto min_x = (a.w + b.w)/2;
            auto min_y = (a.h + b.h)/2;

            return (len_x <= min_x && len_y <= min_y);

        };

        // if the player has hit the top or bottom border, they lose
        if(vars.bird_y < vars.half_height || vars.bird_y > (450 - vars.half_height))
            return true;

        auto col = vars.columns.front();

        // generate shape for bird
        SDL_Rect b;
        b.w = 2 * vars.half_height;
        b.h = 2 * vars.half_height;
        b.y = vars.bird_y - vars.half_height;
        b.x = vars.bird_x;

        // only need to test the front column (because bird never actually moves forwards or backwards)
        SDL_Rect r;
        r.x = col.first;
        r.y = col.second + 10;
        r.h = 600 - r.y;
        r.w = vars.cols_width;

        if(rects_collide(b, r)) return true;

        // top part of column
        r.x = col.first;
        r.w = vars.cols_width;
        r.h = r.y - vars.space_cols_ver;
        r.y = 0;

        return rects_collide(b, r);

    };

    while(vars.running) {

        auto timestamp = SDL_GetTicks();

        switch(currentstate) {
            case STATE.starting:

                // state change may occur during this call:
                sdl_evaluate_events(event_starting_map);
                break;

            case STATE.playing:

                sdl_evaluate_events(event_play_map);

                vars.bird_y   += vars.velocity;
                vars.velocity += vars.gravity;

                // add new column if needed
                {
                    auto r = vars.columns.back();
                    if(r.first < (350 - vars.space_cols_hor)) {
                        vars.columns.push_back({350, (rand() % (450 - 10 - vars.space_cols_ver)) + (5 + vars.space_cols_ver)});
                    }
                }

                // check math on existing columns and remove if needed
                {
                    auto r = vars.columns.front();
                    if(r.first <= -vars.cols_width)
                        vars.columns.pop_front();
                }

                if(player_collides()) currentstate = STATE.endgame;

                // shift all columns left
                for(auto& r : vars.columns)
                    r.first -= vars.col_speed;

                // render sequence
                SDL_FillRect(s, NULL, 0);
                render_cols(s);
                render_base(s);
                render_bird(s);

                break;

            case STATE.endgame:

                sdl_evaluate_events(event_endgame_map);

                // render information from last frame
                SDL_FillRect(s, NULL, 0);
                render_cols(s);
                render_base(s);
                render_bird(s);

                // render red border
                {

                    const int bt = 5; // border thickness
                    auto color = SDL_MapRGB(s->format, 255, 0, 0);

                    SDL_Rect r;
                    r.w = bt;
                    r.h = 600;
                    r.x = 0;
                    r.y = 0;
                    SDL_FillRect(s, &r, color);

                    r.x = 350 - bt;
                    SDL_FillRect(s, &r, color);

                    r.x = 0;
                    r.h = bt;
                    r.w = 350;
                    SDL_FillRect(s, &r, color);

                    r.y = 600 - bt;
                    SDL_FillRect(s, &r, color);

                }

                break;

            default:
                currentstate = STATE.endgame;
                break;
        }

        SDL_Flip(s);
        //auto frame_time = SDL_GetTicks() - timestamp;
        //if(frame_time < 16 && frame_time > 0)
        //    SDL_Delay(16 - frame_time);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_Quit();

    return 0;

}
