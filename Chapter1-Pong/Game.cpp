#include "Game.hpp"

#include <stdexcept>
#include <string>
#include <SDL.h>

Game::Game()
	: m_Window{ nullptr }
	, m_Renderer{ nullptr }
	, m_TickesCount{ 0 }
	, m_IsRunning{ true }
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		throw std::runtime_error{ SDL_GetError()};
	}

	m_Window = SDL_CreateWindow
	(
		"Pong",
		100, 100, 
		WindowWidth, WindowHeight,
		0
	);
	if (!m_Window)
	{
		throw std::runtime_error{ SDL_GetError()};
	}

	m_Renderer = SDL_CreateRenderer
	(
		m_Window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!m_Renderer)
	{
		throw std::runtime_error{ SDL_GetError() };
	}

	m_RightPaddle.Position = { WindowWidth - Thickness, WindowHeight / 2 };
	m_RightPaddle.Direction = 0;

	m_LeftPaddle.Position = { Thickness, WindowHeight / 2 };
	m_LeftPaddle.Direction = 0;
	
	m_Balls.push_back({ { WindowWidth / 2, WindowHeight / 2 }, { 100, 120 }, 0x0000ffff });
	m_Balls.push_back({ { WindowWidth / 2, WindowHeight / 2 }, { -120, 92 }, 0x00ff00ff });
	m_Balls.push_back({ { WindowWidth / 2, WindowHeight / 2 }, { -70, 220 }, 0xff0000ff });
}

Game::~Game()
{
	SDL_DestroyRenderer(m_Renderer);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

void Game::Run()
{
	while (m_IsRunning)
	{
		this->HandleInput();
		this->UpdateGame();
		this->GenerateOutput();
	}
}

void Game::HandleInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: 
			m_IsRunning = false;
			break;
		}
	}

	const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

	if (keyboardState[SDL_SCANCODE_ESCAPE])
	{
		m_IsRunning = false;
	}

	m_RightPaddle.Direction = 0;
	m_LeftPaddle.Direction = 0;

	if (keyboardState[SDL_SCANCODE_I])
	{
		m_RightPaddle.Direction += -1;
	}
	if (keyboardState[SDL_SCANCODE_K])
	{
		m_RightPaddle.Direction += 1;
	}

	if (keyboardState[SDL_SCANCODE_W])
	{
		m_LeftPaddle.Direction += -1;
	}
	if (keyboardState[SDL_SCANCODE_S])
	{
		m_LeftPaddle.Direction +=  1;
	}
}

void Game::UpdateGame()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), m_TickesCount + 16));

	float deltaTime = (SDL_GetTicks() - m_TickesCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	m_TickesCount = SDL_GetTicks();

	// right and left paddle wall collision detection
	PaddleWallCollision(m_RightPaddle, deltaTime);
	PaddleWallCollision(m_LeftPaddle, deltaTime);

	for (auto& ball : m_Balls)
	{
		ball.Position.x += ball.Velocity.x * deltaTime;
		ball.Position.y += ball.Velocity.y * deltaTime;

		// right paddle collisiton detection
		float diff = ball.Position.y - m_RightPaddle.Position.y;
		diff = (diff > 0.0f) ? diff : -diff;

		if (diff < PaddleHeight / 2)
		{
			if (ball.Position.x + Thickness / 2 > m_RightPaddle.Position.x - Thickness / 2  && ball.Velocity.x > 0.0f)
			{
				ball.Velocity.x *= -1.0f;
			}
		}

		// left paddle collisiton detection
		diff = ball.Position.y - m_LeftPaddle.Position.y;
		diff = (diff > 0.0f) ? diff : -diff;

		if (diff < PaddleHeight / 2)
		{
			if (ball.Position.x - Thickness / 2 < m_LeftPaddle.Position.x + Thickness / 2 && ball.Velocity.x < 0.0f)
			{
				ball.Velocity.x *= -1.0f;
			}
		}

		// right and left wall detection
		if (ball.Position.x > WindowWidth - Thickness / 2)
		{
			m_IsRunning = false;
		}
		else if (ball.Position.x < Thickness / 2)
		{
			m_IsRunning = false;
		}

		// upper and lower wall collision detection
		if (ball.Position.y < Thickness + Thickness / 2 && ball.Velocity.y < 0.0f)
		{
			ball.Velocity.y *= -1.0f;
		}
		else if (ball.Position.y > WindowHeight - (Thickness + Thickness / 2) && ball.Velocity.y > 0.0f)
		{
			ball.Velocity.y *= -1.0f;
		}
	}	
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(m_Renderer, 15, 15, 15, 255);
	
	SDL_RenderClear(m_Renderer);

	SDL_SetRenderDrawColor(m_Renderer, 0, 225, 225, 255);

	SDL_Rect rect;

	// render upper wall
	rect = {
		0,
		0,
		WindowWidth,
		Thickness, 
	};
	SDL_RenderFillRect(m_Renderer, &rect);

	// render lower wall 
	rect =
	{
		0,
		WindowHeight - Thickness,
		WindowWidth,
		Thickness,
	};
	SDL_RenderFillRect(m_Renderer, &rect);

	// render right paddle
	rect = 
	{
		static_cast<int>(m_RightPaddle.Position.x) - Thickness / 2,
		static_cast<int>(m_RightPaddle.Position.y) - PaddleHeight / 2,
		Thickness,
		PaddleHeight,
	};
	SDL_RenderFillRect(m_Renderer, &rect);

	// render left paddle
	rect =
	{
		static_cast<int>(m_LeftPaddle.Position.x) - Thickness / 2,
		static_cast<int>(m_LeftPaddle.Position.y) - PaddleHeight / 2,
		Thickness,
		PaddleHeight,
	};
	SDL_RenderFillRect(m_Renderer, &rect);

	// render ball

	for (auto& ball : m_Balls)
	{
		rect =
		{
			static_cast<int>(ball.Position.x) - Thickness / 2,
			static_cast<int>(ball.Position.y) - Thickness / 2,
			Thickness,
			Thickness,
		};

		std::uint8_t red   = (ball.Color & 0xff000000) >> 23;
		std::uint8_t green = (ball.Color & 0x00ff0000) >> 15;
		std::uint8_t blue  = (ball.Color & 0x0000ff00) >> 7;
		std::uint8_t alpha = (ball.Color & 0x000000ff) >> 0;

		SDL_SetRenderDrawColor(m_Renderer, red, green, blue, alpha);
		SDL_RenderFillRect(m_Renderer, &rect);
	}
	

	SDL_RenderPresent(m_Renderer);
}

void Game::PaddleWallCollision(Paddle& paddle, float deltaTime)
{
	if (paddle.Direction)
	{
		paddle.Position.y += paddle.Direction * PaddleSpeed * deltaTime;

		if (paddle.Position.y < PaddleHeight / 2 + Thickness)
		{
			paddle.Position.y = PaddleHeight / 2 + Thickness;
		}
		else if (paddle.Position.y > WindowHeight - PaddleHeight / 2 - Thickness)
		{
			paddle.Position.y = WindowHeight - PaddleHeight / 2 - Thickness;
		}
	}
}