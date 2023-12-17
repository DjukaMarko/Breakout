#include "../includes/StaticBox.hpp"
#include <iostream>

// Constructor
StaticBox::StaticBox(SDL_Renderer* renderer, Uint8 red, Uint8 green, Uint8 blue, int x, int y, int width, int height)
 : x(x), y(y), width(width), height(height), isDestroyed(false) {

    // Create a surface with a solid color
    surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, red, green, blue));

    // Create texture from surface
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        // Handle error accordingly
    }

    std::cout << "StaticBox created!" << std::endl;
}

bool StaticBox::isBoxDestroyed() {
    return isDestroyed;
}

int StaticBox::getX() {
    return x;
}

int StaticBox::getY() {
    return y;
}

int StaticBox::getWidth() {
    return width;
}

int StaticBox::getHeight() {
    return height;
}

// Destructor
StaticBox::~StaticBox() {
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    isDestroyed = true;
}

// Function to draw the StaticBox
void StaticBox::draw(SDL_Renderer *renderer) {
    //std::cout << "StaticBox drawing!" << std::endl;
    SDL_Rect destinationRect = { x, y, width, height };
    SDL_RenderCopy(renderer, texture, nullptr, &destinationRect);
}

bool StaticBox::checkBoundingCollision(int x, int y, int size) {
    // Check collision between this static box and a bounding box defined by x, y, and size
    return (x < this->x + this->width &&
            x + size > this->x &&
            y < this->y + this->height &&
            y + size > this->y);
}