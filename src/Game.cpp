#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../includes/Game.hpp"
#include "../includes/Sprite.hpp"
#include "../includes/StaticBox.hpp"
#include "../includes/Ball.hpp"
#include <vector>
#include <SDL2/SDL_mixer.h>
#include <random>
#include <chrono>

Game::Game() {

}

Game::~Game() {
    Game::cleanup();
}

Ball * Game::getBall() {
    return ball;
}

void Game::init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen) {

    std::cout << "Initialization..." << std::endl;
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    if(SDL_Init(SDL_INIT_EVERYTHING) == 0 ) {
        std::cout << "Subsystems initialized..." << std::endl;

        screenWidth = width;
        screenHeight = height;

        window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if(window) {
            std::cout << "Window created!" << std::endl;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(renderer) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            std::cout << "Renderer created!" << std::endl;
        }

        if (TTF_Init() == -1) {
            std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        }

        font = TTF_OpenFont("../includes/fonts/valorax.ttf", 24);  // Replace with the path to your TrueType font file
        if (!font) {
            std::cerr << "Font loading failed: " << TTF_GetError() << std::endl;
        }


        if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
            std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: %s\n" << Mix_GetError() << std::endl;
        }

        //Load music
        hMusic = Mix_LoadMUS( "../includes/sound_effects/arcade_kid.mp3" );
        if( hMusic == NULL )
        {
            std::cout << "Failed to load beat music! SDL_mixer Error: %s\n" << Mix_GetError() << std::endl ;
        }

        pMusic = Mix_LoadMUS( "../includes/sound_effects/play_game_music.mp3" );
        if( pMusic == NULL )
        {
            std::cout << "Failed to load beat music! SDL_mixer Error: %s\n" << Mix_GetError() << std::endl ;
        }
      
        this->initializePlay();
    } else {
        isRunning = false;
    }
}

void Game::initializePlay() {
    
    // Dimensions of each box
    const int boxWidth = 60;
    const int boxHeight = 15;

    // Number of rows and columns
    const int numRows = 6;
    const int numCols = 8;

    // 6*8 = 48 number of boxes

    // Calculate the starting position to center the array at the top
    int startX = (screenWidth - numCols * (boxWidth + 5)) / 2;
    int startY = 5;  // Top of the screen with a gap of 5 pixels


    staticBoxArray.resize(numRows);

    for (int i = 0; i < numRows; ++i) {
        staticBoxArray[i].resize(numCols);
    }

    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            // Create sprites as needed (you might want to adjust parameters)
            staticBoxArray[i][j] = new StaticBox(renderer, 255, 255, 255, startX + j * (boxWidth + 5), startY + i * (boxHeight + 5), boxWidth, boxHeight);
            // new StaticBox(renderer, 255, 255, 255, i * 65, j * 20, 60, 15);
            std::cout << i << " " << j << std::endl;
        }
    }

    score = 0;
    player_sprite = new Sprite(renderer, 255, 255, 255, (screenWidth/2-50), 300.0f, 90, 15);
    ball = new Ball(renderer, 5, screenWidth/2-50, 280, 1.0, screenWidth, screenHeight);

    startTime = std::chrono::high_resolution_clock::now();
    elapsedTimeSeconds = 0;
    isRunning = true;
    menuNum = 0;
    gameState = GameState::HOME;
}

void Game::destroyMainGame() {
    for (auto& row : staticBoxArray) {
        for (auto staticbox : row) {
            delete staticbox;
        }
        row.clear();
    }
    staticBoxArray.clear();

}

int Game::getElapsedTime() {
    return elapsedTimeSeconds;
}


void Game::updateElapsedTime() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
    elapsedTimeSeconds = elapsedTime.count();
}

GameState Game::getGameState() {
    return gameState;
}

void Game::setGameState(GameState state) {
    gameState = state;
}

int Game::getScore() {
    return score;
}

void Game::increaseScore() {
    score += 1;
}

int Game::getScreenWidth() {
    return screenWidth;
}
int Game::getScreenHeight() {
    return screenHeight;
}


void Game::handleEvents(int delta) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            isRunning = false;
            std::cout << "quit" << std::endl;
            break;
        case SDL_KEYDOWN:
            if(gameState == GameState::HOME) {
                handleHomeKeyDown(event.key);
            } else if (gameState == GameState::PLAY) {
                handleGameKeyDown(event.key, delta);
            }
            break;
        default:
            break;
    }
}

void Game::renderText(const std::string& text, int x, int y, int sizeW, int sizeH, SDL_Color textColor) {
    if(text.empty()) return;
    
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (!textSurface) {
        std::cerr << "Text rendering failed: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    if (!textTexture) {
        std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect renderQuad = { x, y, textSurface->w + sizeW, textSurface->h + sizeH };
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
    SDL_DestroyTexture(textTexture);
}


void Game::handleGameKeyDown(SDL_KeyboardEvent& keyEvent, int delta) {
    switch (keyEvent.keysym.sym) {
        case SDLK_LEFT:
            // Move the sprite to the left if within bounds
            if (player_sprite->getX() >= 0) {
                player_sprite->move(player_sprite->getX() - player_sprite->getMoveSpeed()*delta);
            }
            break;
        case SDLK_RIGHT:
            // Move the sprite to the right if within bounds
            if (player_sprite->getX() + player_sprite->getWidth() <= screenWidth) {
                player_sprite->move(player_sprite->getX() + player_sprite->getMoveSpeed()*delta);
            }
            break;
        case SDLK_ESCAPE:
            gameState = GameState::HOME;
            stopMusic();
            break;
        // Add more cases for other keys if needed
        default:
            break;
    }
}

void Game::handleHomeKeyDown(SDL_KeyboardEvent& keyEvent) {
    switch (keyEvent.keysym.sym) {
        case SDLK_DOWN:
            menuNum = ((menuNum+1) % 2);
            break;
        case SDLK_UP:
            menuNum = abs(((menuNum-1) % 2));
            break;
        case SDLK_RETURN:
            if(!menuNum) {
                stopMusic();
                gameState = GameState::PLAY;
            }
            else isRunning = false;
        // Add more cases for other keys if needed
        default:
            break;
    }
}

void Game::stopMusic() {
    Mix_HaltMusic();
}

void Game::drawHome() {
    SDL_RenderClear(renderer);
    SDL_Color textColor = { 255, 255, 255, 255 }; // White color
    renderText("BREAKOUT", screenWidth/2 - 110, screenHeight/2 - 90, 50, 20, textColor);

    renderText((menuNum == 0 ? ">" : ""), screenWidth/2 - 110, screenHeight/2 - 20, 1, -5, textColor);
    renderText((menuNum == 1 ? ">" : ""), screenWidth/2 - 110, screenHeight/2 + 10, 1, -5, textColor);

    renderText(("Enter Game"), screenWidth/2 - 90, screenHeight/2 - 20, 1, -5, textColor);
    renderText(("Exit"), screenWidth/2 - 90, screenHeight/2 + 10, -10, -5, textColor);
}

void Game::renderHome() {
    SDL_RenderPresent(renderer);
}

void Game::run() {
    Uint32 desired_fps = 60; 
    Uint32 last_ticks = SDL_GetTicks();
    
    while (this->isRunning) {
        Uint32 delta = SDL_GetTicks() - last_ticks;
        if (delta < 1000/desired_fps) {
            continue;
        } 
        last_ticks = SDL_GetTicks();

        Mix_VolumeMusic(MIX_MAX_VOLUME);  // Set music volume to maximum
        this->updateElapsedTime();
        switch (gameState) {
            case GameState::HOME:

                if (Mix_PlayingMusic() == 0) {
                    Mix_PlayMusic(hMusic, -1);  // -1 for looping
                }
                
                this->handleEvents(delta);
                this->drawHome();
                this->renderHome();
                break;

            case GameState::PLAY:
                if (Mix_PlayingMusic() == 0) {
                    Mix_PlayMusic(pMusic, -1);  // -1 for looping
                }
                this->handleEvents(delta);
                this->drawGame();
                this->renderGame();
                this->getBall()->move(this);
                break;
        }

    }
    this->cleanup();
}

int Game::generateRandomNumber() {
    std::random_device rd;  // Obtain a random seed from the operating system
    std::mt19937 gen(rd());  // Mersenne Twister engine

    // Define a distribution (e.g., uniform distribution between 1 and 100)
    std::uniform_int_distribution<> distribution(1, 100);


    return distribution(gen);
}

void Game::update() {
}

Sprite *Game::getSprite() {
    return player_sprite;
}

void Game::drawGame()
{
    SDL_RenderClear(renderer);
    player_sprite->draw(renderer);
    ball->draw(renderer);
    for (const auto& row : staticBoxArray) {
        for (const auto& staticbox : row) {
            staticbox->draw(renderer);
        }
    }


    SDL_Color textColor = { 255, 255, 255, 255 }; // Black color
    renderText("Score: " + to_string(score), 10, screenHeight-50, 20, 0, textColor);
}

vector<vector<StaticBox*>> Game::getStaticBoxArray() {
    return staticBoxArray;
}

void Game::renderGame() {
    SDL_RenderPresent(renderer);
}


void Game::reset() {
    this->setGameState(GameState::HOME);
    this->stopMusic();
    this->initializePlay();
}

void Game::cleanup() {
    // Clean up individual resources

    for (auto& row : staticBoxArray) {
        for (auto staticbox : row) {
            delete staticbox;
        }
        row.clear();
    }
    staticBoxArray.clear();

    Mix_FreeChunk(gHit);
    gHit = nullptr;
    
    Mix_FreeMusic(hMusic);
    hMusic = nullptr;

    Mix_FreeMusic(pMusic);
    pMusic = nullptr;

    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Clean up SDL subsystems
    Mix_Quit();
    SDL_Quit();

    std::cout << "Game cleaned!" << std::endl;
}