#pragma once
#include <SFML/Graphics.hpp>
#include "Math.h"
#include "Constants.h"


    struct Brick;
    struct Game;
    struct Apple
    {
        
        Vector2D applesPositions; // Массив позиций яблок
        sf::Sprite applesSprite;

       
    };


    void InitApples(Apple& apples, const Game& game,Brick & bricks);

