#include <iostream>
#include "../includes/Game.hpp"
#include <SDL2/SDL.h>

Game *game = nullptr;

int main (int argc, char *argv[]) {

    game = new Game();
    game->init("Breakout", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, false);
    game->run();

    
    return 0;
}