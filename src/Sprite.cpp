#include "../includes/Sprite.hpp"
#include "../includes/StaticBox.hpp"
#include <iostream>

// Constructor
Sprite::Sprite(SDL_Renderer* renderer, Uint8 red, Uint8 green, Uint8 blue, float x, float y, int width, int height)
 : StaticBox(renderer, red, green, blue, x, y, width, height) {

    // Set a speed for sprite while moving left and right with arrows

    moveSpeed = 1.0;
    std::cout << "Sprite created!" << std::endl;
}

void Sprite::move(float newX) {
    // Move the sprite to the new position
    x = newX;
}

float Sprite::getMoveSpeed() {
    return moveSpeed;
}

// Destructor
Sprite::~Sprite() {
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// Function to draw the sprite
void Sprite::draw(SDL_Renderer *renderer) {
    //std::cout << "Sprite drawing!" << std::endl;
    SDL_Rect destinationRect = { x, y, width, height };
    SDL_RenderCopy(renderer, texture, nullptr, &destinationRect);
}
