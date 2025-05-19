#pragma once
#include <SFML/Graphics.hpp>
#include "Math.h"
#include "Constants.h"


    struct Game;
    enum class PlayerDirection
    {
        Right = 0,
        Up,
        Left,
        Down
    };

    struct Player
    {
        std::vector<Vector2D> bodyPositions; // Добавлено определение bodyPositions
        std::vector<sf::Sprite> bodySprites;
        Vector2D playerPosition;
        float playerSpeed = INITIAL_SPEED;
        PlayerDirection playerDirection = PlayerDirection::Right;
        sf::Vector2f baseScale;
    };



    void InitPlayer(Player& player, const Game& game);
    void HandleInput(Game& game, Player& player);
    void RotateAccordingToDirection( Player& player);



