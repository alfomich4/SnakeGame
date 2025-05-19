#include "Game.h"
#include "Math.h"
#include "Record.h"
#include <cassert>
#include <algorithm>
#include <sstream>
#include <array>


 // --- Стек состояний ---
GameState currentState(const Game& g)
{
    return g.stateStack.empty() ? GameState::None : g.stateStack.back();
}

void pushState(Game& g, GameState s)
{
    g.stateStack.push_back(s);
}

void popState(Game& g)
{
    if (!g.stateStack.empty())
    {
        g.stateStack.pop_back();
    }
}

void changeState(Game& g, GameState s)
{
    if (!g.stateStack.empty())
        g.stateStack.back() = s;
    else
        g.stateStack.push_back(s);
}



void InitGame(Player& player, Apple& apples, Brick& bricks, Game& game, const sf::Font& font)
{
    assert(game.headTexture.loadFromFile(RESOURCES_PATH + "snake/head_right.png"));
    assert(game.bodyTexture.loadFromFile(RESOURCES_PATH + "snake/body_horizontal.png"));
    assert(game.tailTexture.loadFromFile(RESOURCES_PATH + "snake/tail_left.png"));
    assert(game.appleTexture.loadFromFile(RESOURCES_PATH + "apple.png"));
    assert(game.brickTexture.loadFromFile(RESOURCES_PATH + "pink_brick.png"));
    assert(game.eatBuffer.loadFromFile(RESOURCES_PATH + "Snake_hit.wav"));
    assert(game.deathBuffer.loadFromFile(RESOURCES_PATH + "Lose.wav"));
    assert(game.backGroundBuffer.loadFromFile(RESOURCES_PATH + "Background_Music.wav"));
    assert(game.enterBuffer.loadFromFile(RESOURCES_PATH + "Enter.wav"));
    assert(game.menuHoverBuffer.loadFromFile(RESOURCES_PATH + "menu-hover.wav"));

    game.eatSound.setBuffer(game.eatBuffer);
    game.deathSound.setBuffer(game.deathBuffer);
    game.backGroundSound.setBuffer(game.backGroundBuffer);
    game.enterSound.setBuffer(game.enterBuffer);
    game.menuHoverSound.setBuffer(game.menuHoverBuffer);

    InitPlayer(player, game);
    InitApples(apples, game,bricks);
    InitBricks(bricks, game);

    game.stateStack.clear();
    pushState(game, GameState::ModeSelection);
    game.selectedMode = 0;
    game.selectedPause = 0;

    game.numEatenApples = 0;
    game.gameFinishTime = 0.f;
    game.gameMode = GameModes::EASY;

    game.background.setSize({ SCREEN_WIDTH, SCREEN_HEIGHT });
    game.background.setFillColor(sf::Color::Black);

    game.scoreText.setFont(font);
    game.scoreText.setCharacterSize(20);
    game.scoreText.setFillColor(sf::Color::White);
    game.scoreText.setPosition(10, 10);

    game.inputHintText.setFont(font);
    game.inputHintText.setCharacterSize(20);
    game.inputHintText.setFillColor(sf::Color::Yellow);
    game.inputHintText.setString("Use arrow keys to move, press space to restart, ESC to exit");
    game.inputHintText.setPosition(200, 10);

    game.gameOverText.setFont(font);
    game.gameOverText.setCharacterSize(70);
    game.gameOverText.setFillColor(sf::Color::Cyan);
    game.gameOverText.setPosition(200, 200);

    game.leadertable.setFont(font);
    game.leadertable.setCharacterSize(24);
    game.leadertable.setFillColor(sf::Color::White);

    game.RestartMessage.setFont(font);
    game.RestartMessage.setCharacterSize(20);
    game.RestartMessage.setFillColor(sf::Color::White);
    game.RestartMessage.setPosition(200, 280);
    game.RestartMessage.setString("Press Space to Restart");

    game.backspaceHintText.setFont(font);
    game.backspaceHintText.setCharacterSize(20);
    game.backspaceHintText.setFillColor(sf::Color::Yellow);
    game.backspaceHintText.setPosition(200, 500);
    game.backspaceHintText.setString("Press BackSpace to return to menu");

    // Настройка меню паузы
    static const std::array<std::string, Game::PAUSE_COUNT> pauseLabels = { "Resume", "Main Menu" };
    for (int i = 0; i < Game::PAUSE_COUNT; ++i)
    {
        game.pauseMenuText[i].setFont(font);
        game.pauseMenuText[i].setCharacterSize(30);
        game.pauseMenuText[i].setString(pauseLabels[i]);
    }




}





void UpdateGame(Game& game, Player& player, Apple& apples, Brick& bricks, float deltaTime)
{
    if (currentState(game) != GameState::Playing) return;
    Vector2D prevHeadPos = player.bodyPositions[0];
    // движение игрока
    switch (player.playerDirection)
    {
    case PlayerDirection::Right: player.bodyPositions[0].x += player.playerSpeed * deltaTime; break;
    case PlayerDirection::Up:    player.bodyPositions[0].y -= player.playerSpeed * deltaTime; break;
    case PlayerDirection::Left:  player.bodyPositions[0].x -= player.playerSpeed * deltaTime; break;
    case PlayerDirection::Down:  player.bodyPositions[0].y += player.playerSpeed * deltaTime; break;
    }

    // Двигаем всё тело: каждый сегмент идёт на место предыдущего
    for (std::size_t i = 1; i < player.bodyPositions.size(); ++i)
    {
        Vector2D temp = player.bodyPositions[i];
        player.bodyPositions[i] = prevHeadPos;
        prevHeadPos = temp;
    }

    // столкновение с кирпичами

    Vector2D headPos = player.bodyPositions[0];

    for (std::size_t i = 0; i < bricks.brickSprite.size(); ++i)
    {
        sf::Vector2f brickPos = bricks.brickSprite[i].getPosition();
        Vector2D brickCenter = { brickPos.x, brickPos.y };

        if (IsCirclesCollide(headPos, PLAYER_SIZE / 2.f, brickCenter, BRICK_SIZE / 2.f))
        {
            changeState(game, GameState::GameOver);
            game.deathSound.play();
            break;
        }
    }

    for (std::size_t i = 1; i < player.bodyPositions.size(); ++i) // начинаем с 1, чтобы пропустить голову
    {
        Vector2D segmentPos = player.bodyPositions[i];

        if (IsCirclesCollide(headPos, PLAYER_SIZE / 2.f, segmentPos, PLAYER_SIZE / 2.f))
        {
            changeState(game, GameState::GameOver);
            game.deathSound.play();
            break;
        }
    }
    // столкновение с яблоком
    if (IsCirclesCollide(player.playerPosition, PLAYER_SIZE / 2,
        apples.applesPositions, APPLE_SIZE / 2))
    {
        ++game.numEatenApples;
        apples.applesPositions = GetRandomPositionInScreen();
        apples.applesSprite.setPosition(apples.applesPositions.x, apples.applesPositions.y);
    }
    
}

void RenderGame(sf::RenderWindow& window, Game& game, Player& player, Apple& apples, Brick& bricks, const sf::Font& font)
{
    window.clear();
    switch (currentState(game))
    {
    case GameState::ModeSelection:
        ShowModeSelectionScreen(window, game,font); 
        break;
    case GameState::Playing:
        game.background.setFillColor(sf::Color::Black);
        window.draw(game.background);
    }
   
        
        
}
    



void HandleModeSelection(Game& game, const sf::Event& event,Player & player)
{
    if (currentState(game) != GameState::ModeSelection || event.type != sf::Event::KeyPressed) return;
    switch (event.key.code)
    {
    case sf::Keyboard::Up:
        game.selectedMode = (game.selectedMode + Game::MODE_COUNT - 1) % Game::MODE_COUNT;
        break;
    case sf::Keyboard::Down:
        game.selectedMode = (game.selectedMode + 1) % Game::MODE_COUNT;
        break;
    case sf::Keyboard::Return:

        switch (game.selectedMode)
        {
        case 0: game.gameMode = GameModes::EASY; pushState(game, GameState::Playing); player.playerSpeed = 100.f; break;
        case 1: game.gameMode = GameModes::HEAVIER_THAN_EASY; pushState(game, GameState::Playing); player.playerSpeed = 150.f; break;
        case 2: game.gameMode = GameModes::MEDIUM; pushState(game, GameState::Playing); player.playerSpeed = 200.f; break;
        case 3: game.gameMode = GameModes::LIGHTER_THAN_HEAVY; pushState(game, GameState::Playing); player.playerSpeed = 250.f; break;
        case 4: game.gameMode = GameModes::HARD; pushState(game, GameState::Playing); break;
        case 5: pushState(game, GameState::Leaderboard); break;
        }
        break;
    default:
        break;
    }
}

// --- Обработка паузы ---
void HandlePauseInput(Game& game, const sf::Event& event)
{
    if (currentState(game) == GameState::Playing &&
        event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::P)
    {
        pushState(game, GameState::Paused);
    }
    else if (currentState(game) == GameState::Paused && event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::Up:
        case sf::Keyboard::Down:
            game.selectedPause = (game.selectedPause + 1) % Game::PAUSE_COUNT;
            break;
        case sf::Keyboard::Return:

            if (game.selectedPause == 0)
            {
                popState(game);  // Resume
            }
            else
                while (currentState(game) != GameState::ModeSelection)
                {
                    popState(game); // Main Menu
                }
            break;

        default:
            break;
        }
    }
}

void HandleLeaderboardInput(Game& game, const sf::Event& event)
{
    if (currentState(game) == GameState::Leaderboard &&
        event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::BackSpace)
    {
        popState(game);
    }
}


void ShowModeSelectionScreen(sf::RenderWindow& window, Game& game, const sf::Font& font)
{
    static const std::array<std::string, Game::MODE_COUNT> labels =
    {
        "Start Game",
        "Difficult",
        "Leaderboard",
        "Settings",
        "Exit",
        
    };
    sf::Text title;
    title.setFont(font);
    title.setString("SnakeGame");
    title.setCharacterSize(40);
    title.setFillColor(sf::Color::White);
    title.setPosition(250, 100);
    window.draw(title);

    for (int i = 0; i < Game::MODE_COUNT; ++i) 
    {
        sf::Text txt;
        txt.setFont(font);
        txt.setString(labels[i]);
        txt.setCharacterSize(30);
        txt.setFillColor(i == game.selectedMode ? sf::Color::Green : sf::Color::White);
        txt.setPosition(150, 200 + i * 50);
        window.draw(txt);
    }
}


void ShowLeaderboardScreen(sf::RenderWindow& window, Game& game)
{

}




void ShowPauseScreen(sf::RenderWindow& window, Game& game, const sf::Font& font)
{
    sf::RectangleShape overlay({ float(SCREEN_WIDTH), float(SCREEN_HEIGHT) });
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    sf::Text title;
    title.setFont(font);
    title.setString("Paused");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color::White);
    title.setPosition(300, 150);
    window.draw(title);

    for (int i = 0; i < Game::PAUSE_COUNT; ++i)
    {
        auto& txt = game.pauseMenuText[i];
        txt.setFillColor(i == game.selectedPause ? sf::Color::Red : sf::Color::Green);
        txt.setPosition(350, 250 + i * 50);
        window.draw(txt);
    }
}

void ShowDifficultyScreen(sf::RenderWindow& window, Game& game, const sf::Font& font)
{
    static const std::array<std::string, Game::DIFF_COUNT> labels =
    {
        "EASY",
        "HEAVIER_THAN_EASY",
        "MEDIUM",
        "LIGHTER_THAN_HEAVY",
        "HARD",

    };

    sf::Text title;
    title.setFont(font);
    title.setString("DIFFICULTS");
    title.setCharacterSize(40);
    title.setFillColor(sf::Color::White);
    title.setPosition(250, 100);
    window.draw(title);
    for (int i = 0; i < Game::DIFF_COUNT; ++i)
    {
        sf::Text txt;
        txt.setFont(font);
        txt.setString(labels[i]);
        txt.setCharacterSize(30);
        txt.setFillColor(i == game.selectedDiff ? sf::Color::Green : sf::Color::White);
        txt.setPosition(150, 200 + i * 50);
        window.draw(txt);
    }
}





