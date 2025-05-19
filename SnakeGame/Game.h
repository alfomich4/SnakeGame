#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "Apple.h"
#include "Brick.h"
#include "Constants.h"

enum class GameState
{
    None = 0,
    ModeSelection,
    Playing,
    Paused,        
    GameOver,
    Leaderboard
};
struct Game
{
    int numEatenApples = 0;
    float gameFinishTime = 0.f;
    GameModes gameMode = GameModes::EASY;
    
    // ���� ���������
    std::vector<GameState> stateStack;

    // ���� ������ ������: 5 ������� ������� + 1 ����� �Leaderboard�
    static constexpr int MODE_COUNT = 5;
    int selectedMode = 0;
    // ���� ������ ��������� ����
    static constexpr int DIFF_COUNT = 5;
    int selectedDiff = 0;

    // ���� �����: ��� ������ �Resume� � �Main Menu�
    static constexpr int PAUSE_COUNT = 2;
    int selectedPause = 0;
    sf::Text pauseMenuText[PAUSE_COUNT];

    // ����������� ��������
    sf::RectangleShape background;
    sf::Text scoreText;
    sf::Text inputHintText;
    sf::Text gameOverText;
    sf::Text leadertable;
    sf::Text RestartMessage;
    sf::Text backspaceHintText;

    sf::Texture headTexture;
    sf::Texture bodyTexture;
    sf::Texture tailTexture;
    sf::Texture appleTexture;
    sf::Texture brickTexture;

    sf::SoundBuffer eatBuffer;
    sf::SoundBuffer deathBuffer;
    sf::SoundBuffer backGroundBuffer;
    sf::SoundBuffer enterBuffer;
    sf::SoundBuffer menuHoverBuffer;
    
    sf::Sound eatSound;
    sf::Sound deathSound;
    sf::Sound backGroundSound;
    sf::Sound enterSound;
    sf::Sound menuHoverSound;
   

    // ������ �� ������ ���������
   
    
};

GameState currentState(const Game& g);
void pushState(Game& g, GameState s);
void popState(Game& g);
void changeState(Game& g, GameState s);


// �������� ������� ����
void InitGame(Player& player, Apple& apples, Brick& bricks, Game& game, const sf::Font& font);
void UpdateGame(Game& game, Player& player, Apple& apples, Brick & bricks, float deltaTime);
void RenderGame(sf::RenderWindow& window, Game& game, Player& player, Apple& apples, Brick& bricks, const sf::Font& font);

// ����������� �����
void HandleModeSelection(Game& game, const sf::Event& event, Player& player);
void HandleLeaderboardInput(Game& game, const sf::Event& event);
void HandlePauseInput(Game& game, const sf::Event& event);

// �������� ���������
void ShowModeSelectionScreen(sf::RenderWindow& window, Game& game, const sf::Font& font);
void ShowLeaderboardScreen(sf::RenderWindow& window, Game& game);
void ShowPauseScreen(sf::RenderWindow& window, Game& game,const sf:: Font & font);
void ShowDifficultyScreen(sf::RenderWindow& window, Game& game, const sf::Font& font);




