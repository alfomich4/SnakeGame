#pragma once
# include<string>


	enum class GameModes
	{
		EASY = 0,
		HEAVIER_THAN_EASY = 1,
		MEDIUM = 2,
		LIGHTER_THAN_HEAVY = 3,
		HARD = 4,

	};
	const std::string RESOURCES_PATH = "Resources/";
	const std::string LEADERBOARD_FILE = "leaderboard.txt";
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	const float INITIAL_SPEED = 100.f;
	const float PLAYER_SIZE = 20.f;
	const int NUM_APPLES = 1;
	const float APPLE_SIZE = 20.f;
	const float ROCK_SIZE = 10.f;
	const float PAUSE_LENGTH = 2.f;
	const float BRICK_SIZE = 20.f;
	const int NUM_BRICKS_HORIZONTAL = 40;   // 800 / 20 = 40
	const int NUM_BRICKS_VERTICAL = 30; // 600 / 20 = 30
	const int TOTAL_NUM_BRICKS = 136; // 40+30+40+30 - 4 corners= 136
	const float PLAYER_TEXTURE_SIZE = 40.f;
	const float APPLE_TEXTURE_SIZE = 40.f;
	const float BRICK_TEXTURE_SIZE = 8.f;

