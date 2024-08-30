#include <iostream>
#include <raylib.h>

using namespace std;

const int SPEED_BALL = 7;
const int SPEED_PALLE = 10;
const int SMART_CPU = 10;

// Setting color
Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

int player_score = 0;
int cpu_score = 0;

class Paddle;

class Ball
{
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw()
    {
        DrawCircle(x, y, radius, Yellow);
    }

    void Update(Sound sound)
    {
        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0)
        {
            speed_y *= -1;
            PlaySound(sound);
        }

        if (x + radius >= GetScreenWidth()) // Cpu wins
        {
            cpu_score++;
            ResetBall();
        }
        if (x - radius <= 0) // Player wins
        {
            player_score++;
            ResetBall();
        }
    }

    void ResetBall()
    {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        speed_x = SPEED_BALL;
        speed_y = SPEED_BALL;

        int speed_choises[2] = {-1, 1};
        speed_x *= speed_choises[GetRandomValue(0, 1)];
        speed_y *= speed_choises[GetRandomValue(0, 1)];
    }
};

class Paddle
{
protected:
    void LimitMovement()
    {
        if (y <= 0)
            y = 0;
        if (y + height >= GetScreenHeight())
            y = GetScreenHeight() - height;
    }

public:
    float x, y;
    float width, height;
    int speed;

    void Draw()
    {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }

    void Update()
    {
        if (IsKeyDown(KEY_UP))
            y -= speed;
        if (IsKeyDown(KEY_DOWN))
            y += speed;
        LimitMovement();
    }
};

class CpuPaddle : public Paddle
{
public:
    void Update(int ball_y)
    {
        if (y + height / 2 > ball_y)
            y -= speed;
        if (y + height / 2 <= ball_y)
            y += speed;
        LimitMovement();
    }
};

Ball ball;
Paddle player;
CpuPaddle cpu;

int main()
{

    cout << "Starting the game" << endl;
    const int screen_width = 1500;
    const int screen_height = 800;
    // Tạo màn hình
    InitWindow(screen_width, screen_height, "My Pong Game!");
    // Instal Sounds
    InitAudioDevice();
    Sound Soundtrack = LoadSound("Sound\\chi.mp3");
    Sound PongSound = LoadSound("Sound\\blomp.mp3");
    SetSoundVolume(Soundtrack, 0.5);
    PlaySound(Soundtrack);
    SetSoundVolume(PongSound, 3);

    SetTargetFPS(60);

    // Create the ball
    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = SPEED_BALL;
    ball.speed_y = SPEED_BALL;

    // Create the player
    player.width = 32;
    player.height = 150;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = SPEED_PALLE;

    // Create the cpu
    cpu.width = 32;
    cpu.height = 150;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = SMART_CPU;

    int check = 0; // Check whether y has been increased or not

    // Game Loop
    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        // Updatemain
        ball.Update(PongSound);
        player.Update();
        cpu.Update(ball.y);

        int Game_Difficulty = 0;

        // Checking for collisions
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height}))
        {
            ball.speed_x *= -1;
            PlaySound(PongSound);

            // Effectively
            if ((player.y <= ball.y && ball.y < player.y + 30) || (player.y + 120 <= ball.y && ball.y <= player.y + 150))
                if (!check)
                {
                    ball.speed_y += ball.speed_y > 0 ? 5 : -5;
                    check = 1;
                }

            if (player.y + 30 <= ball.y && ball.y < player.y + 120)
                if (check)
                {
                    ball.speed_y -= ball.speed_y > 0 ? 5 : -5;
                    check = 0;
                }

            // Increate difficulty
            ball.speed_x += ball.speed_x > 0 ? 1 : -1;
            ball.speed_y += ball.speed_y > 0 ? 1 : -1;

            if (!Game_Difficulty % 5)
                cpu.speed++;
        }

        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
        {
            ball.speed_x *= -1;
            PlaySound(PongSound);

            // Effectively
            if ((cpu.y <= ball.y && ball.y < cpu.y + 30) || (cpu.y + 120 <= ball.y && ball.y <= cpu.y + 150))
                if (!check)
                {
                    ball.speed_y += ball.speed_y > 0 ? 5 : -5;
                    check = 1;
                }

            if (cpu.y + 30 <= ball.y && ball.y < cpu.y + 120)
                if (check)
                {
                    ball.speed_y -= ball.speed_y > 0 ? 5 : -5;
                    check = 0;
                }
        }

        // Drawing
        ClearBackground(Dark_Green);
        DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
        DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
        ball.Draw();
        cpu.Draw();
        player.Draw();
        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

        EndDrawing();
    }
    UnloadSound(Soundtrack);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}