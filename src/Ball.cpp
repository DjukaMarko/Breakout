// Ball.cpp
#include "../includes/Ball.hpp"
#include "../includes/Game.hpp"
#include "../includes/Sprite.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <random>
#include <thread>
#include <mutex>

Ball::Ball(SDL_Renderer *renderer, int size, int initialX, int initialY, float speed, int screenWidth, int screenHeight)
    : size(size), x(initialX), y(initialY), speed(speed), velocityX(speed+1), velocityY(-speed-1), screenWidth(screenWidth), screenHeight(screenHeight) {

        // Create a surface with a solid color

    surface = SDL_CreateRGBSurface(0, size, size, 32, 0, 0, 0, 0);
    SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 255, 255, 255));

    // Create texture from surface
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        // Handle error accordingly
    }

    speedIncreased = false;
    increaseSpeedAsync();
    std::cout << "Ball created!" << std::endl;

}

int Ball::generateRandomNumber() {
    std::random_device rd;  // Obtain a random seed from the operating system
    std::mt19937 gen(rd());  // Mersenne Twister engine

    // Define a distribution (e.g., uniform distribution between 1 and 100)
    std::uniform_int_distribution<> distribution(1, 100);

    return distribution(gen);
}

Ball::~Ball() {
    // Clean up if needed
}

void Ball::move(Game *game) {
    // Update the ball's position based on its velocity
    //std::cout << "X: " << x << ", Y:" << y << std::endl;

    prevX = x;
    prevY = y;

    x += velocityX*speed;
    y += velocityY*speed;


    // Increase speed after 15 seconds
    std::cout << "Elapsed Time: " << game->getElapsedTime() << std::endl;

    // Handle collisions with walls
    if (x <= 0 || x + size >= screenWidth) {
        velocityX = -velocityX; // Reverse X velocity for a bounce
    }

    if (y <= 0) {
        velocityY = -velocityY; // Reverse Y velocity for a bounce
    }

    if(y + size >= screenHeight) {
        game->reset();
    }

    for (const auto& row : game->getStaticBoxArray()) {
            for (const auto& box : row ) {
                int collisionType = game->getBall()->handleCollision(box, game->getSprite());
                if(collisionType == 1) {
                    game->increaseScore();
                    int numRows = game->getStaticBoxArray().size();
                    int numCols = game->getStaticBoxArray()[0].size();
                    if(game->getScore() == (numRows*numCols)) game->reset();
                }
            }
        }
}

void Ball::draw(SDL_Renderer *renderer) {
    // Draw the square ball on the renderer
    SDL_Rect destinationRect = { x, y, size, size };
    SDL_RenderCopy(renderer, texture, nullptr, &destinationRect);
}

// Handle collision with a static box
/*
* 0 - no collision
* 1 - collision with StaticBox
* 2 - collision with Sprite
*/
int Ball::handleCollision(StaticBox *box, Sprite *player_sprite) {
    // Check if there is a collision with the bounding box of the static box
    if (box->checkBoundingCollision(x, y, size)) {

        if(box->isBoxDestroyed()) {
            return 0;
        }

        // Random num of bouncing ball back where it came from (1/4 chance)
        int randomNum = this->generateRandomNumber();
        // Calculate the center of the ball
        int ballCenterX = x + size / 2;
        int ballCenterY = y + size / 2;

        // Calculate the center of the box
        int boxCenterX = box->getX() + box->getWidth() / 2;
        int boxCenterY = box->getY() + box->getHeight() / 2;

        // Calculate the difference in position
        int deltaX = ballCenterX - boxCenterX;
        int deltaY = ballCenterY - boxCenterY;

        // Calculate the sum of half-widths and half-heights
        int halfWidths = (size + box->getWidth()) / 2;
        int halfHeights = (size + box->getHeight()) / 2;

        // Check if the absolute difference is less than the sum of half-widths and half-heights
        if (abs(deltaX) < halfWidths && abs(deltaY) < halfHeights) {
            // Collision occurred

            // Calculate overlap on each axis
            int overlapX = halfWidths - abs(deltaX);
            int overlapY = halfHeights - abs(deltaY);

            // Determine the side of the box with the smaller overlap
            if (overlapX < overlapY) {
                // Adjust the ball's position based on the overlap
                x += (deltaX > 0) ? overlapX : -overlapX;

                // Reverse X velocity for a bounce

                if(randomNum > 10) {
                    velocityX = -velocityX;
                }
            } else {
                // Adjust the ball's position based on the overlap
                y += (deltaY > 0) ? overlapY : -overlapY;

                // Reverse Y velocity for a bounce
                if(randomNum > 10) {
                    velocityY = -velocityY;
                }
            }
        }
        box->~StaticBox();
        return 1;
    }


    if (player_sprite->checkBoundingCollision(x, y, size)) {

        // Calculate the center of the ball
        int ballCenterX = x + size / 2;
        int ballCenterY = y + size / 2;

        // Calculate the center of the box
        int spriteCenterX = player_sprite->getX() + player_sprite->getWidth() / 2;
        int spriteCenterY = player_sprite->getY() + player_sprite->getHeight() / 2;

        // Calculate the difference in position
        int deltaX = ballCenterX - spriteCenterX;
        int deltaY = ballCenterY - spriteCenterY;

        // Calculate the sum of half-widths and half-heights
        int halfWidths = (size + player_sprite->getWidth()) / 2;
        int halfHeights = (size + player_sprite->getHeight()) / 2;

        // Check if the absolute difference is less than the sum of half-widths and half-heights
        if (abs(deltaX) < halfWidths && abs(deltaY) < halfHeights) {
            // Collision occurred

            // Calculate overlap on each axis
            int overlapX = halfWidths - abs(deltaX);
            int overlapY = halfHeights - abs(deltaY);

            // Determine the side of the box with the smaller overlap
            if (overlapX < overlapY) {
                // Adjust the ball's position based on the overlap
                x += (deltaX > 0) ? overlapX : -overlapX;
                velocityX = -velocityX;
            } else {
                // Adjust the ball's position based on the overlap
                y += (deltaY > 0) ? overlapY : -overlapY;
                velocityY = -velocityY;
            }
        }
        std::cout << this->velocityX << std::endl;
        return 2;
    }
    return 0;
}

void Ball::increaseSpeedAsync() {
    std::unique_lock<std::mutex> lock(speedMutex);
    if(!speedIncreased) {
        std::thread speedIncrementThread([this]() {
            std::this_thread::sleep_for(std::chrono::seconds(15));
            this->increaseVelocity();
            std::cout << "Speed increased after 15 seconds!" << std::endl;
            speedIncreased = true;
        });

        speedIncrementThread.detach();
    }
}

void Ball::increaseVelocity() {
    speed += 0.5;
}

void Ball::decreaseVelocity() {
    speed -= 0.5;
}

