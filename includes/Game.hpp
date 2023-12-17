#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Sprite.hpp"
#include "StaticBox.hpp"
#include "Ball.hpp"
#include <string>
#include <vector>
#include <SDL2/SDL_mixer.h>
#include <chrono>

using namespace std;

class Ball;

enum GameState {
    HOME,
    PLAY
};

class Game {
public:
    // Constructors and Destructors
    Game();
    ~Game();

    // Initialization and Cleanup
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void cleanup();

    // Main Game Loop
    void run();
    void initializePlay();
    void destroyMainGame();
    // Event Handling
    void handleEvents(int delta);
    void handleGameKeyDown(SDL_KeyboardEvent& keyEvent, int delta);

    // Rendering Home Game Loop
    void renderHome();
    void drawHome();
    void handleHomeKeyDown(SDL_KeyboardEvent& keyEvent);


    GameState getGameState();
    void setGameState(GameState state);
    // Rendering Main Game Loop
    void renderGame();
    void stopMusic();
    void drawGame();
    void renderText(const string& text, int x, int y, int sizeW, int sizeH, SDL_Color textColor);
    void reset();
    // Getters
    Sprite* getSprite();
    Ball* getBall();
    vector<vector<StaticBox*>> getStaticBoxArray();
    int getScreenWidth();
    int getScreenHeight();
    int getScore();

    void updateElapsedTime();
    int getElapsedTime();

    // Game Logic
    void update();
    void increaseScore();
    int generateRandomNumber(); // integer between 1 and 100

private:
    // SDL-related members
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    Mix_Music* hMusic;
    Mix_Music* pMusic;
    Mix_Chunk* gHit;


    chrono::time_point<chrono::high_resolution_clock> startTime;
    int elapsedTimeSeconds;


    // Home state
    int menuNum;

    // Game state
    int score;
    int screenWidth;
    int screenHeight;
    bool isRunning;

    // Game objects
    Sprite* player_sprite;
    Ball* ball;

    // Game entities
    vector<vector<StaticBox*>> staticBoxArray;
    GameState gameState;
};

#endif // GAME_H
