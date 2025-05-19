#pragma once
#include "Constants.h"
#include <SFML/Graphics.hpp>



    struct Vector2D
    {
        float x = 0;
        float y = 0;
    };
    std::vector<Vector2D> GetHorizontalBrickPositions();
    std::vector<Vector2D> GetVerticalBrickPositions();
    Vector2D GetRandomPositionInScreen();
    bool IsCirclesCollide(Vector2D pos1, float radius1, Vector2D pos2, float radius2);
    void setSpriteSize(sf::Sprite& sprite, float desiredWidth, float desiredHeight);

  


