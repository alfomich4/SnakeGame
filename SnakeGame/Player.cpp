#include "Player.h"
#include "Constants.h"
#include "Game.h"
#include <SFML/Graphics.hpp>


void AddSegment(Player& player, const sf::Texture& texture, const Vector2D& pos)
{
    player.bodyPositions.push_back(pos);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setOrigin(PLAYER_TEXTURE_SIZE / 2.f, PLAYER_TEXTURE_SIZE / 2.f);
    sprite.setPosition(pos.x, pos.y);
    setSpriteSize(sprite, PLAYER_TEXTURE_SIZE, PLAYER_SIZE);

    player.bodySprites.push_back(sprite);
}

void InitPlayer(Player& player, const Game& game)
{
    player.bodyPositions.clear();
    player.bodySprites.clear();

    Vector2D headPos = { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f };
    const int initialLength = 3;

    for (int i = 0; i < initialLength; ++i)
    {
        Vector2D pos = { headPos.x - i * PLAYER_SIZE, headPos.y };

        if (i == 0)
            AddSegment(player, game.headTexture, pos);
        else if (i == initialLength - 1)
            AddSegment(player, game.tailTexture, pos);
        else
            AddSegment(player, game.bodyTexture, pos);
    }

    player.playerSpeed = INITIAL_SPEED;
    player.playerDirection = PlayerDirection::Right;
    player.baseScale = player.bodySprites[0].getScale();
}

void HandleInput(Game& game, Player& player)
{
    if (player.bodySprites.empty() || player.bodyPositions.size() < 2)
        return;

    // === ГОЛОВА ===
    sf::Sprite& headSprite = player.bodySprites[0];

    switch (player.playerDirection)
    {
    case PlayerDirection::Right:
        headSprite.setRotation(0.f);
        headSprite.setScale(player.baseScale.x, player.baseScale.y);
        break;
    case PlayerDirection::Up:
        headSprite.setRotation(270.f);
        headSprite.setScale(player.baseScale.x, player.baseScale.y);
        break;
    case PlayerDirection::Down:
        headSprite.setRotation(90.f);
        headSprite.setScale(player.baseScale.x, player.baseScale.y);
        break;
    case PlayerDirection::Left:
        headSprite.setRotation(0.f);
        headSprite.setScale(-player.baseScale.x, player.baseScale.y);
        break;
    }

    // === ХВОСТ ===
    // направление от предпоследнего к последнему
    Vector2D tailFrom = player.bodyPositions[player.bodyPositions.size() - 2];
    Vector2D tailTo = player.bodyPositions[player.bodyPositions.size() - 1];
    sf::Sprite& tailSprite = player.bodySprites.back();

    Vector2D diff = { tailTo.x - tailFrom.x, tailTo.y - tailFrom.y };

    if (diff.x > 0)
    {
        tailSprite.setRotation(0.f);
        tailSprite.setScale(player.baseScale.x, player.baseScale.y);
    }
    else if (diff.x < 0)
    {
        tailSprite.setRotation(0.f);
        tailSprite.setScale(-player.baseScale.x, player.baseScale.y);
    }
    else if (diff.y > 0)
    {
        tailSprite.setRotation(90.f);
        tailSprite.setScale(player.baseScale.x, player.baseScale.y);
    }
    else if (diff.y < 0)
    {
        tailSprite.setRotation(270.f);
        tailSprite.setScale(player.baseScale.x, player.baseScale.y);
    }

    // === ТЕЛО (опционально): можно реализовать позже, если хочешь правильные изгибы
}





void RotateAccordingToDirection(Player& player)
{
    
    
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            player.playerDirection = PlayerDirection::Right;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            player.playerDirection = PlayerDirection::Up;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            player.playerDirection = PlayerDirection::Left;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            player.playerDirection = PlayerDirection::Down;
    

}


