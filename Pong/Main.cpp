#include <iostream>
#include <raylib.h>

int playerScore = 0, cpuScore = 0;

Color green = Color{38, 185, 154, 255};
Color darkGreen = Color{ 20, 160, 133, 255 };
Color lightGreen = Color{ 129, 204, 184, 255 };
Color yellow = Color{ 243, 213, 91, 255 };
Color shadow = Color{ 42, 42, 42, 100 };



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
		
		int randNum = rand() % 2;
		speedX = startVel[randNum][0];
		speedY = startVel[randNum][1];

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
		}
		else if (position.x + radius >= GetScreenWidth())
		{
			Reset();
			playerScore++;
		}
		else if (position.x - radius <= 0)
		{
			Reset();
			cpuScore++;
		}
		
	}

	void Reset()
	{
		position.x = GetScreenWidth() / 2;
		position.y = GetScreenHeight() / 2;
		
		int randNum = rand() & 2;

		speedX = startVel[randNum][0];
		speedY = startVel[randNum][1];
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

int main(int argc, char* argv[])
{
	const unsigned int screenWidth = 1280, screenHeight = 800;

	InitWindow(screenWidth, screenHeight, "Pong");
	SetTargetFPS(60);
	srand(time(0));

	Ball ball = Ball(20);
	Paddle player = Paddle(10, screenHeight / 2 - 60);
	CPUPaddle cpu = CPUPaddle(screenWidth - 35, screenHeight / 2 - 60);

	while (!WindowShouldClose())
	{
		ball.Update();
		player.Update();
		cpu.Update(ball.GetPosition().y);

		if (CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), Rectangle{ player.GetPosition().x, player.GetPosition().y, player.GetProportions().x, player.GetProportions().y }))
		{
			ball.InvertSpeedX();
		}
		else if (CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), Rectangle{ cpu.GetPosition().x, cpu.GetPosition().y, cpu.GetProportions().x, cpu.GetProportions().y }))
		{
			ball.InvertSpeedX();
		}
		
		BeginDrawing();

		ClearBackground(darkGreen);

		DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, green);
		DrawCircle(screenWidth / 2, screenHeight / 2, 150, lightGreen);
		DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE);
		DrawText(TextFormat("%i", cpuScore), screenWidth / 2 + 10, screenHeight / 2 - 40, 80, WHITE);
		DrawText(TextFormat("%i", playerScore), screenWidth / 2 - 50, screenHeight / 2 - 40, 80, WHITE);
		ball.Draw();
		player.Draw();
		cpu.Draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}