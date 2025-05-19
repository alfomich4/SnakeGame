#include "Math.h"
#include <cstdlib>
#include <SFML/Graphics.hpp>

std::vector<Vector2D> GetHorizontalBrickPositions()
{
    std::vector<Vector2D> positions;

    for (int i = 0; i < NUM_BRICKS_HORIZONTAL; ++i)
    {
        positions.push_back({ i * BRICK_SIZE, 0 }); // верх
        positions.push_back({ i * BRICK_SIZE, SCREEN_HEIGHT - BRICK_SIZE }); // низ
    }

    return positions;
}

std::vector<Vector2D> GetVerticalBrickPositions()
{
    std::vector<Vector2D> positions;

    for (int i = 1; i < NUM_BRICKS_VERTICAL - 1; ++i)
    {
        positions.push_back({ 0, i * BRICK_SIZE }); // слева
        positions.push_back({ SCREEN_WIDTH - BRICK_SIZE, i * BRICK_SIZE }); // справа
    }

    return positions;
}
Vector2D GetRandomPositionInScreen()
{
    Vector2D result;
    result.x = rand() / (float)RAND_MAX * SCREEN_WIDTH;
    result.y = rand() / (float)RAND_MAX * SCREEN_HEIGHT;
    return result;
}

bool IsCirclesCollide(Vector2D pos1, float radius1, Vector2D pos2, float radius2)
{
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    float distanceSquared = dx * dx + dy * dy;
    float radiusSum = radius1 + radius2;
    return distanceSquared <= (radiusSum * radiusSum);
}

void setSpriteSize(sf::Sprite& sprite, float textureSize, float desiredSize)
{
    float scale = desiredSize / textureSize;
    sprite.setScale(scale, scale);
}