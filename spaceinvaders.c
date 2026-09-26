#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ROWS 4
#define COLS 10

typedef struct {
    Vector2 pos;
    bool active;
} Alien;

typedef struct {
    Vector2 pos;
    bool active;
    float speed;
} Bullet;

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders");
    SetTargetFPS(0);

    Vector2 player = {
        SCREEN_WIDTH / 2.0f - 20,
        SCREEN_HEIGHT - 50
    };

    const float playerSpeed = 5.0f;

    Alien aliens[ROWS][COLS];

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            aliens[row][col].pos = (Vector2){
                100 + col * 60,
                80 + row * 45
            };
            aliens[row][col].active = true;
        }
    }

    float alienDirection = 1.0f;
    float alienSpeed = 35.0f;
    float alienDrop = 20.0f;

    Bullet playerBullet = {
        .pos = {0, 0},
        .active = false,
        .speed = 7.0f
    };

    Bullet enemyBullets[20] = {0};

    float shootTimer = 0.0f;

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_A))
            player.x -= playerSpeed;

        if (IsKeyDown(KEY_D))
            player.x += playerSpeed;

        if (player.x < 0)
            player.x = 0;

        if (player.x > SCREEN_WIDTH - 40)
            player.x = SCREEN_WIDTH - 40;

        if (IsKeyPressed(KEY_SPACE) && !playerBullet.active)
        {
            playerBullet.pos = (Vector2){
                player.x + 18,
                player.y - 8
            };

            playerBullet.active = true;
        }

        if (playerBullet.active)
        {
            playerBullet.pos.y -= playerBullet.speed;

            if (playerBullet.pos.y < 0)
                playerBullet.active = false;
        }

        bool hitEdge = false;

        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                if (!aliens[row][col].active)
                    continue;

                aliens[row][col].pos.x +=
                    alienDirection * alienSpeed * GetFrameTime();

                if (aliens[row][col].pos.x < 20 ||
                    aliens[row][col].pos.x > SCREEN_WIDTH - 40)
                {
                    hitEdge = true;
                }
            }
        }

        if (hitEdge)
        {
            alienDirection *= -1;

            for (int row = 0; row < ROWS; row++)
            {
                for (int col = 0; col < COLS; col++)
                {
                    if (aliens[row][col].active)
                        aliens[row][col].pos.y += alienDrop;
                }
            }
        }

        if (playerBullet.active)
        {
            for (int row = 0; row < ROWS; row++)
            {
                for (int col = 0; col < COLS; col++)
                {
                    if (!aliens[row][col].active)
                        continue;

                    Rectangle alienRect = {
                        aliens[row][col].pos.x,
                        aliens[row][col].pos.y,
                        30,
                        25
                    };

                    Rectangle bulletRect = {
                        playerBullet.pos.x - 2,
                        playerBullet.pos.y - 6,
                        4,
                        12
                    };

                    if (CheckCollisionRecs(alienRect, bulletRect))
                    {
                        aliens[row][col].active = false;
                        playerBullet.active = false;
                        alienSpeed += 2.0f;
                        break;
                    }
                }
            }
        }

        shootTimer += GetFrameTime();

        if (shootTimer > 0.8f)
        {
            shootTimer = 0;

            for (int attempts = 0; attempts < 20; attempts++)
            {
                int row = GetRandomValue(0, ROWS - 1);
                int col = GetRandomValue(0, COLS - 1);

                if (aliens[row][col].active)
                {
                    for (int i = 0; i < 20; i++)
                    {
                        if (!enemyBullets[i].active)
                        {
                            enemyBullets[i].active = true;
                            enemyBullets[i].speed = 4.0f;

                            enemyBullets[i].pos = (Vector2){
                                aliens[row][col].pos.x + 15,
                                aliens[row][col].pos.y + 25
                            };

                            break;
                        }
                    }

                    break;
                }
            }
        }

        for (int i = 0; i < 20; i++)
        {
            if (!enemyBullets[i].active)
                continue;

            enemyBullets[i].pos.y += enemyBullets[i].speed;

            if (enemyBullets[i].pos.y > SCREEN_HEIGHT)
                enemyBullets[i].active = false;
        }

        BeginDrawing();

        ClearBackground(BLACK);

        for (int row = 0; row < ROWS; row++)
        {
            for (int col = 0; col < COLS; col++)
            {
                if (!aliens[row][col].active)
                    continue;

                Vector2 p = aliens[row][col].pos;

                DrawRectangle((int)p.x + 5, (int)p.y, 20, 5, GREEN);
                DrawRectangle((int)p.x, (int)p.y + 5, 30, 15, GREEN);
                DrawRectangle((int)p.x + 5, (int)p.y + 20, 5, 5, GREEN);
                DrawRectangle((int)p.x + 20, (int)p.y + 20, 5, 5, GREEN);

                DrawRectangle((int)p.x + 7, (int)p.y + 7, 4, 4, BLACK);
                DrawRectangle((int)p.x + 19, (int)p.y + 7, 4, 4, BLACK);
            }
        }

        DrawRectangle(
            (int)player.x + 15,
            (int)player.y - 10,
            10,
            10,
            WHITE
        );

        DrawRectangle(
            (int)player.x,
            (int)player.y,
            40,
            10,
            WHITE
        );

        if (playerBullet.active)
        {
            DrawRectangle(
                (int)playerBullet.pos.x - 2,
                (int)playerBullet.pos.y - 6,
                4,
                12,
                WHITE
            );
        }

        for (int i = 0; i < 20; i++)
        {
            if (enemyBullets[i].active)
            {
                DrawRectangle(
                    (int)enemyBullets[i].pos.x - 2,
                    (int)enemyBullets[i].pos.y,
                    4,
                    10,
                    WHITE
                );
            }
        }

        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, GREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

