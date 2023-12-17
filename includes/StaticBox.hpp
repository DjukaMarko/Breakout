#ifndef STATICBOX_HPP
#define STATICBOX_HPP

#include <SDL2/SDL.h>

class StaticBox {
public:
    // Constructor for solid color sprite
    StaticBox(SDL_Renderer* renderer, Uint8 red, Uint8 green, Uint8 blue, int x, int y, int width, int height);

    // Destructor
    ~StaticBox();

    // Function to draw the sprite
    void draw(SDL_Renderer *renderer);
    bool checkBoundingCollision(int x, int y, int size);
    int getX();
    int getY();
    int getWidth();
    int getHeight();
    bool isBoxDestroyed();

    // Other member functions for sprite manipulation, animation, etc.


protected:
    SDL_Surface* surface;
    SDL_Texture* texture;
    int x, y, width, height;


private:
    bool isDestroyed;

};

#endif // STATICBOX
