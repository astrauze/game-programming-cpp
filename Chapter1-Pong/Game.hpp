#ifndef GAME_HPP
#define GAME_HPP

#include <cstdint>
#include <vector>

struct Vector2
{
	float x;
	float y;
};

struct Ball
{
	Vector2 Position;
	Vector2 Velocity;
	std::uint32_t Color;
};

struct Paddle
{
	Vector2 Position;
	int Direction;
};

class Game
{
public:
	Game();
	~Game();

	void Run();

private:
	void HandleInput();
	void UpdateGame();
	void GenerateOutput();

	void PaddleWallCollision(Paddle& paddle, float deltaTime);

	struct SDL_Window* m_Window;
	struct SDL_Renderer* m_Renderer;
	std::uint32_t m_TickesCount;
	bool m_IsRunning;

	static constexpr int WindowWidth = 1024;
	static constexpr int WindowHeight = 768;

	static constexpr int Thickness = 15;
	static constexpr int PaddleHeight = 100;
	static constexpr float PaddleSpeed = 300.0f;

	Paddle m_RightPaddle;
	Paddle m_LeftPaddle;
	
	std::vector<Ball> m_Balls;
};

#endif // !GAME_HPP