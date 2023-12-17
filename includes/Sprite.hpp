#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <SDL2/SDL.h>
#include "StaticBox.hpp"


class Sprite : public StaticBox {
public:
    // Constructor for solid color sprite
    Sprite(SDL_Renderer* renderer, Uint8 red, Uint8 green, Uint8 blue, float x, float y, int width, int height);

    // Destructor
    ~Sprite();

    // Function to draw the sprite
    void draw(SDL_Renderer *renderer);
    void move(float newX);
    float getMoveSpeed();

    // Other member functions for sprite manipulation, animation, etc.

private:
    int moveSpeed;
};

#endif // SPRITE_HPP