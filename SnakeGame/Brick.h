#pragma once
#include <SFML/Graphics.hpp>
#include "Math.h"



    struct Game;
    struct Brick
    {
        std::vector<Vector2D> brickPositionsHorizontal;
        std::vector<Vector2D> brickPositionsVertical;
        std::vector<sf::Sprite> brickSprite;
    };

     void InitBricks(Brick& bricks,const Game & game);


