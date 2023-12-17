// Ball.hpp
#ifndef BALL_HPP
#define BALL_HPP

#include <SDL2/SDL.h>
#include "StaticBox.hpp"
#include "Game.hpp"
#include <mutex>

class Game;

class Ball {
public:
    Ball(SDL_Renderer *renderer, int size, int initialX, int initialY, float speed, int screenWidth, int screenHeight);
    ~Ball();

    void move(Game *game);
    void draw(SDL_Renderer *renderer);
    int handleCollision(StaticBox* box, Sprite *player_sprite);
    int generateRandomNumber();
    void increaseVelocity();
    void decreaseVelocity();
    void increaseSpeedAsync();


private:
    SDL_Surface* surface;
    SDL_Texture* texture;
    int size;     // Size of the square ball
    int x, y;     // Position of the ball's top-left corner
    float speed;    // Movement speed
    float velocityX; // Velocity in the X direction
    float velocityY; // Velocity in the Y direction
    int prevX, prevY;
    int screenWidth; // Screen sizes for checking collision with walls
    int screenHeight;
    bool speedIncreased;
    std::mutex speedMutex;
};

#endif // BALL_HPP
