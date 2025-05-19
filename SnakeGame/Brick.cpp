#include "Brick.h"
#include "Constants.h"
#include "Game.h"
#include "Math.h"



void InitBricks(Brick& bricks, const Game& game)
{
    // Получаем координаты
    bricks.brickPositionsHorizontal = GetHorizontalBrickPositions();
    bricks.brickPositionsVertical = GetVerticalBrickPositions();

    // Объединяем все позиции
    std::vector<Vector2D> allPositions = bricks.brickPositionsHorizontal;
    allPositions.insert(allPositions.end(), bricks.brickPositionsVertical.begin(), bricks.brickPositionsVertical.end());

    // Инициализируем спрайты
    bricks.brickSprite.resize(allPositions.size());

    for (std::size_t i = 0; i < allPositions.size(); ++i)
    {
        bricks.brickSprite[i].setTexture(game.brickTexture);
        bricks.brickSprite[i].setOrigin(BRICK_TEXTURE_SIZE / 2.f, BRICK_TEXTURE_SIZE / 2.f);
        bricks.brickSprite[i].setPosition(allPositions[i].x, allPositions[i].y);
        setSpriteSize(bricks.brickSprite[i], BRICK_TEXTURE_SIZE, BRICK_SIZE);
    }
}



