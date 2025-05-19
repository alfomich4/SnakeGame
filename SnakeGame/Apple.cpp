#include "Apple.h"
#include "Constants.h"
#include "Game.h"
#include "Brick.h"



void InitApples(Apple& apples, const Game& game, Brick& bricks)
{
    // ќбъедин€ем все кирпичные позиции
    std::vector<Vector2D> allBrickPositions = bricks.brickPositionsHorizontal;
    allBrickPositions.insert(allBrickPositions.end(), bricks.brickPositionsVertical.begin(), bricks.brickPositionsVertical.end());

    // √енерируем позицию €блока вне кирпичей
    Vector2D position;
    bool valid = false;

    while (!valid)
    {
        position = GetRandomPositionInScreen();
        valid = true;

        for (const auto& brickPos : allBrickPositions)
        {
            if (brickPos.x == position.x && brickPos.y == position.y)
            {
                valid = false;
                break;
            }
        }
    }

    // ”становка текстуры и спрайта
    apples.applesPositions = position;
    apples.applesSprite.setTexture(game.appleTexture);
    apples.applesSprite.setOrigin(APPLE_TEXTURE_SIZE / 2.f, APPLE_TEXTURE_SIZE / 2.f);
    apples.applesSprite.setPosition(position.x, position.y);
    setSpriteSize(apples.applesSprite, APPLE_TEXTURE_SIZE, APPLE_SIZE);

    
}
