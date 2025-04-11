#include <iostream>
#include <raylib.h>

int playerScore = 0, cpuScore = 0;

Color green = Color{38, 185, 154, 255};
Color darkGreen = Color{ 20, 160, 133, 255 };
Color lightGreen = Color{ 129, 204, 184, 255 };
Color yellow = Color{ 243, 213, 91, 255 };
Color shadow = Color{ 42, 42, 42, 100 };
Color pauseShadow = Color{ 42, 42, 42, 175 };

enum GameState
{
	STATE_MENU,
	STATE_GAME,
	STATE_PAUSE,
};

Sound scoreSound, wallHitSound;

class Ball
{

	Vector2 position;
	float speedX, speedY;
	int radius;
	int startVel[2][2] = {
		{7, 7},
		{-7, -7},
	};

public:
	Ball(int newRad)
	{
		position.x = GetScreenWidth() / 2;
		position.y = GetScreenHeight() / 2;
		
		int randNum = rand() % 1;
		speedX = startVel[0][0];
		speedY = startVel[0][1];

		radius = newRad;
	}

	void Draw()
	{
		DrawCircle(position.x + 10, position.y + 10, radius, shadow);
		DrawCircle(position.x, position.y, radius, yellow); 
	}

	void Update()
	{
		position.x += speedX;
		position.y += speedY;

		if (position.y + radius >= GetScreenHeight() || position.y - radius <= 0)
		{
			InvertSpeedY();
			PlaySound(wallHitSound);
		}
		else if (position.x + radius >= GetScreenWidth())
		{
			Reset();
			playerScore++;
			PlaySound(scoreSound);
		}
		else if (position.x - radius <= 0)
		{
			Reset();
			cpuScore++;
			PlaySound(scoreSound);
		}
		
	}

	void Reset()
	{
		position.x = GetScreenWidth() / 2;
		position.y = GetScreenHeight() / 2;
		
		int randNum = rand() & 2;

		speedX = startVel[0][0];
		speedY = startVel[0][1];
	}

	void InvertSpeed()
	{
		speedX = -speedX;
		speedY = -speedY;
	}

	void InvertSpeedX()
	{
		speedX = -speedX;
	}

	void InvertSpeedY()
	{
		speedY = -speedY;
	}

	Vector2 GetPosition()
	{
		return position;
	}

	int GetRadius()
	{
		return radius;
	}
};

class Paddle
{
protected:
	Vector2 Position;
	Vector2 proportions;
	float speedY;

	void LimitMovement()
	{
		if (Position.y <= 0)
		{
			Position.y = 0;
		}
		else if (Position.y >= GetScreenHeight() - proportions.y)
		{
			Position.y = GetScreenHeight() - proportions.y;
		}
	}

public:
	Paddle(float posX, float posY)
	{
		Position = {posX, posY};
		proportions = { 25, 120 };
		speedY = 7;
	}
	
	void Draw()
	{
		Rectangle rec = Rectangle{Position.x, Position.y, proportions.x, proportions.y};
		DrawRectangleRounded(Rectangle{ rec.x + 10, rec.y + 10, rec.width, rec.height }, 0.8, 0, shadow);
		DrawRectangleRounded(rec, 0.8, 0, WHITE);
	}

	void Update()
	{
		if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
		{
			Position.y -= speedY;
		}
		else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
		{
			Position.y += speedY;
		}
		LimitMovement();
	}

	void Reset(int x)
	{
		Position.x = x;
		Position.y = GetScreenHeight() / 2 - 120 / 2;
	}

	Vector2 GetPosition()
	{
		return Position;
	}

	Vector2 GetProportions()
	{
		return proportions;
	}
};

class CPUPaddle : public Paddle
{
public:
	CPUPaddle(float posX, float posY) : Paddle(posX, posY)
	{

	}

	void Update(int ballY)
	{
		if (Position.y + proportions.y / 2 > ballY)
		{
			Position.y -= speedY;
		}
		else if (Position.y + proportions.y / 2 < ballY)
		{
			Position.y += speedY;
		}
		LimitMovement();
	}
};

class Button
{
	Vector2 position;
	Vector2 proportions;
	std::string text;
	Color regColour, hoverColour;
	bool isHovered;

public:
	Button(Vector2 pos, Vector2 size, std::string butText, Color normCol, Color hovCol)
	{
		position = pos;
		proportions = size;
		text = butText;
		regColour = normCol;
		hoverColour = hovCol;
		isHovered = false;
	}

	void Draw()
	{
		(isHovered) ? DrawRectangle(position.x, position.y, proportions.x, proportions.y, regColour) : DrawRectangle(position.x, position.y, proportions.x, proportions.y, hoverColour);
		DrawText(text.c_str(), position.x + proportions.x / 2 - MeasureText(text.c_str(), 75) / 2, position.y + proportions.y / 2 - 75 / 2, 75, WHITE);
	}

	void Draw(Vector2 Offset)
	{
		(isHovered) ? DrawRectangle(position.x + Offset.x, position.y + Offset.y, proportions.x, proportions.y, regColour) : DrawRectangle(position.x + Offset.x, position.y + Offset.y, proportions.x, proportions.y, hoverColour);
		DrawText(text.c_str(), position.x + proportions.x / 2 - MeasureText(text.c_str(), 75) / 2 + Offset.x, position.y + proportions.y / 2 - 75 / 2 + Offset.y, 75, WHITE);
	}

	void Update()
	{
		Vector2 mousePos = GetMousePosition();

		if (CheckCollisionPointRec(mousePos, Rectangle{ position.x, position.y, proportions.x, proportions.y }))
		{
			isHovered = true;
		}
		else
		{
			isHovered = false;
		}
	}

	void Update(Vector2 offset)
	{
		Vector2 mousePos = GetMousePosition();

		if (CheckCollisionPointRec(mousePos, Rectangle{ position.x + offset.x, position.y + offset.y, proportions.x, proportions.y }))
		{
			isHovered = true;
		}
		else
		{
			isHovered = false;
		}
	}

	bool isMouseHovered()
	{
		return isHovered;
	}
};

void restart(Ball &ball, Paddle &player, Paddle &cpu)
{
	playerScore = 0;
	cpuScore = 0;
	ball.Reset();
	player.Reset(10);
	cpu.Reset(GetScreenWidth() - 35);
}

int WinMain(int argc, char* argv[])
{
	const unsigned int screenWidth = 1280, screenHeight = 800;
	Image icon = LoadImage("Images/Pong.png");

	InitWindow(screenWidth, screenHeight, "Pong");
	InitAudioDevice();
	SetWindowIcon(icon);
	UnloadImage(icon);
	SetTargetFPS(60);
	srand(time(0));
	SetExitKey(KEY_NULL);

	GameState state = STATE_MENU;

	Button playButton = Button(Vector2{ screenWidth / 2 - 300 / 2, screenHeight / 2 - 125 / 2 }, Vector2{ 300, 125 }, "PLAY!", green, lightGreen);
	Button restartButton = Button(Vector2{ screenWidth / 2 - 300 / 2, screenHeight / 2 - 125 / 2 + 150 }, Vector2{ 300, 125 }, "Restart", green, lightGreen);
	Button quitButton = Button(Vector2{ screenWidth / 2 - 300 / 2, screenHeight / 2 - 125 / 2 + 150 }, Vector2{ 300, 125 }, "quit", green, lightGreen);

	Ball ball = Ball(20);
	Paddle player = Paddle(10, screenHeight / 2 - 60);
	CPUPaddle cpu = CPUPaddle(screenWidth - 35, screenHeight / 2 - 60);

	Sound paddleHitSound = LoadSound("Sound/PaddleHit.wav");
	wallHitSound = LoadSound("Sound/WallHit.wav");
	scoreSound = LoadSound("Sound/Score.wav");

	while (!WindowShouldClose())
	{
		if (state == STATE_MENU)
		{
			playButton.Update();
			if (playButton.isMouseHovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				state = STATE_GAME;
			}
			quitButton.Update();
			if (quitButton.isMouseHovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				break;
			}
		}
		
		else if (state == STATE_GAME)
		{
			ball.Update();
			player.Update();
			cpu.Update(ball.GetPosition().y);

			if (CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), Rectangle{ player.GetPosition().x, player.GetPosition().y, player.GetProportions().x, player.GetProportions().y }))
			{
				ball.InvertSpeedX();
				PlaySound(paddleHitSound);
			}
			else if (CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), Rectangle{ cpu.GetPosition().x, cpu.GetPosition().y, cpu.GetProportions().x, cpu.GetProportions().y }))
			{
				ball.InvertSpeedX();
				PlaySound(paddleHitSound);
			}

			if (IsKeyPressed(KEY_ESCAPE))
			{
				state = STATE_PAUSE;
			}
		}

		else if (state == STATE_PAUSE)
		{
			playButton.Update();
			if (playButton.isMouseHovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				state = STATE_GAME;
			}

			restartButton.Update();
			if (restartButton.isMouseHovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				restart(ball, player, cpu);
				state = STATE_GAME;
			}

			quitButton.Update(Vector2{0, 150});
			if (quitButton.isMouseHovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				break;
			}
			
			if (IsKeyPressed(KEY_ESCAPE))
			{
				state = STATE_GAME;
			}
		}
		
		BeginDrawing();

		ClearBackground(darkGreen);

		if (state == STATE_MENU)
		{
			//DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, WHITE);
			//DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE);
			//Vector2 mousePos = GetMousePosition();
			//DrawText(TextFormat("X: %f", mousePos.x), 0, 0, 50, WHITE);
			//DrawText(TextFormat("Y: %f", mousePos.y), 0, 55, 50, WHITE);
			// 809, 68
			
			DrawText("PONG!", screenWidth / 2 - 180, 90, 120, WHITE);
			playButton.Draw();
			quitButton.Draw();
			DrawCircle(818 + 10, 61 + 10, 20, shadow);
			DrawCircle(818, 61, 20, yellow);
		}

		else if (state == STATE_GAME || state == STATE_PAUSE)
		{
			DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, green);
			DrawCircle(screenWidth / 2, screenHeight / 2, 150, lightGreen);
			DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE);
			DrawText(TextFormat("%i", cpuScore), screenWidth / 2 + 10, screenHeight / 2 - 40, 80, WHITE);
			DrawText(TextFormat("%i", playerScore), screenWidth / 2 - 50, screenHeight / 2 - 40, 80, WHITE);
			ball.Draw();
			player.Draw();
			cpu.Draw();

			if (state == STATE_PAUSE)
			{
				DrawRectangle(0, 0, screenWidth, screenHeight, pauseShadow);
				DrawText("PONG!", screenWidth / 2 - 180, 90, 120, WHITE);
				DrawCircle(818 + 10, 61 + 10, 20, shadow);
				DrawCircle(818, 61, 20, yellow);
				playButton.Draw();
				restartButton.Draw();
				quitButton.Draw(Vector2{0, 150});
			}
		}

		EndDrawing();
	}

	CloseAudioDevice();
	CloseWindow();
	return 0;
}