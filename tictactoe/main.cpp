#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <sdl_core.h>

using namespace std;

const int None  = 0;
const int PlayX = 1;
const int PlayO = 2;

int game_winner = None;
// index as [y][x]
int _field[9] = {
    None, None, None, 
    None, None, None, 
    None, None, None
};

int wins[8][3] = {
    {0, 3, 6}, 
    {1, 4, 7},
    {2, 5, 8},
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
    {0, 4, 8},
    {2, 4, 6}
};

int game_over(void) {
    for(int i = 0; i < 8; i++) {
        if(
                _field[wins[i][0]] == None || 
                _field[wins[i][2]] == None || 
                _field[wins[i][2]] == None)
            continue;

        if(
                _field[wins[i][0]] == _field[wins[i][1]] && 
                _field[wins[i][1]] == _field[wins[i][2]])    
            return i;
    }

    return -1;
}

int& field(int y, int x) {
    return _field[3*y + x];
}

bool is_free(int y, int x) {
    return field(y, x) == None;
}

void print_x_at(SDL_Surface* s, int y, int x) {
    y *= 200;
    y += 100;
    x *= 200;
    x += 100;

    int hs = 75;

    lineRGBA(s, x-hs, y-hs, x+hs, y+hs, 255, 0, 0, 255);
    lineRGBA(s, x+hs, y-hs, x-hs, y+hs, 255, 0, 0, 255);
}

void print_o_at(SDL_Surface* s, int y, int x) {
    y *= 200;
    y += 100;
    x *= 200;
    x += 100;

    int hs = 75;

    circleRGBA(s, x, y, hs, 0, 0, 255, 255);

}

void print_field_lines(SDL_Surface* s) {

    SDL_Rect r;
    r.x = 200;
    r.y = 0;
    r.h = 600; 
    r.w = 1;
    SDL_FillRect(s, &r, SDL_MapRGB(s->format, 255, 255, 255));

    r.x = 400;
    SDL_FillRect(s, &r, SDL_MapRGB(s->format, 255, 255, 255));

    r.x = 0;
    r.h = 1;
    r.w = 600;
    r.y = 200;
    SDL_FillRect(s, &r, SDL_MapRGB(s->format, 255, 255, 255));

    r.y = 400;
    SDL_FillRect(s, &r, SDL_MapRGB(s->format, 255, 255, 255));

}

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    auto* s = SDL_SetVideoMode(600, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);

    struct {
        bool running = true;
        int square_x, square_y;
        bool has_new = false;
    } vars;

    sdl_event_map_t sdl_event_map = {
        {
            SDL_KEYDOWN,
            [&vars](void* ptr) {
                auto* key_event = reinterpret_cast<SDL_KeyboardEvent*>(ptr);
                if(key_event->keysym.sym == SDLK_ESCAPE)
                    vars.running = false;
            }
        },
        {
            SDL_MOUSEBUTTONDOWN,
            [&vars](void* ptr) {
                auto* mouse_event = reinterpret_cast<SDL_MouseButtonEvent*>(ptr);

                vars.square_x = mouse_event->x / 200;
                vars.square_y = mouse_event->y / 200;
                vars.has_new = true;
            }
        }
    };

    const int STATE_waitforplayer_x = 0;
    const int STATE_waitforplayer_o = 1;
    const int STATE_gameover        = 2;
    int current_state = STATE_waitforplayer_x;

    while(vars.running) {
        sdl_evaluate_events(sdl_event_map);

        // print the current state
        switch(current_state) {
            case STATE_waitforplayer_x: cout << "STATE_waitforplayer_x\n"; break;
            case STATE_waitforplayer_o: cout << "STATE_waitforplayer_0\n"; break;
            case STATE_gameover:         cout << "STATE_gameover\n"; break;
            default: throw runtime_error("unknown internal state"); break;
        }

        // print the state of the board right away
        for(int i = 0; i < 9; i++) {
            cout << _field[i];
            if((i%3) == 2) cout << endl;
        }

        // game logic
        switch(current_state) {
            case STATE_waitforplayer_x:
            
                if(vars.has_new && is_free(vars.square_y, vars.square_x)) {
                    // valid move for player x
                    field(vars.square_y, vars.square_x) = PlayX;
                    current_state = STATE_waitforplayer_o;
                }
                
                {
                    auto win_res = ::game_over();
                    cout << "win_res: " << win_res << endl;
                    if(win_res != -1) {
                        current_state = STATE_gameover;
                        game_winner = _field[wins[win_res][0]];
                    }
                }

                vars.has_new = false;
                
                break;

            case STATE_waitforplayer_o:
            
                if(vars.has_new && is_free(vars.square_y, vars.square_x)) {
                    // valid move for player o
                    field(vars.square_y, vars.square_x) = PlayO;
                    current_state = STATE_waitforplayer_x;
                }

                {
                    auto win_res = ::game_over();
                    cout << "win_res: " << win_res << endl;
                    if(win_res != -1) {
                        current_state = STATE_gameover;
                        game_winner = _field[wins[win_res][0]];
                    }
                }

                vars.has_new = false;

                break;

            // do nothing if game is over
            case STATE_gameover:
                break;

            default:
                vars.running = false;
                break;
        }

        // render the field
        SDL_FillRect(s, NULL, 0);
        ::print_field_lines(s);

        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                if(field(j, i) == PlayX)
                    ::print_x_at(s, j, i);
                if(field(j, i) == PlayO)
                    ::print_o_at(s, j, i);
            }
        }

        // if there is a winner, print the corresponding border color
        if(current_state == STATE_gameover) {
            auto color = (
                game_winner == PlayX ? 
                    SDL_MapRGB(s->format, 255, 0 ,0) : 
                    SDL_MapRGB(s->format, 0, 0, 255));

            int bt = 5;

            // create correct SDL_Rect objs and draw border
            SDL_Rect r;
            r.x = 0;
            r.y = 0;
            r.h = 600;
            r.w = bt;
            SDL_FillRect(s, &r, color);

            r.h = bt;
            r.w = 600;
            SDL_FillRect(s, &r, color);

            r.y = 600 - bt;
            SDL_FillRect(s, &r, color);

            r.y = 0;
            r.x = 600 - bt;
            r.h = 600;
            r.w = bt;
            SDL_FillRect(s, &r, color);
        }

        SDL_Flip(s);
        SDL_Delay(25);
    }

    SDL_Quit();

    return 0;
}

