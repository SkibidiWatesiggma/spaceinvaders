#include "raylib.h"
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_LEVEL 50
#define MAX_ALIENS 50
#define MAX_BULLETS 30

typedef struct {
    Vector2 pos;
    bool active;
} Alien;

typedef struct {
    Vector2 pos;
    bool active;
    float speed;
} Bullet;

static int LoadLevel(void)
{
    FILE *file = fopen("level.txt", "r");

    if (!file)
        return 1;

    int completedLevel = 0;

    if (fscanf(file, "%d", &completedLevel) != 1)
        completedLevel = 0;

    fclose(file);

    if (completedLevel < 0)
        completedLevel = 0;

    if (completedLevel >= MAX_LEVEL)
        return MAX_LEVEL;

    return completedLevel + 1;
}

static void SaveLevel(int level)
{
    FILE *file = fopen("level.txt", "w");

    if (!file)
        return;

    fprintf(file, "%d\n", level);

    fclose(file);
}

static void SetupLevel(
    Alien aliens[],
    int alienCount,
    float *alienSpeed,
    float *alienDirection
)
{
    for (int i = 0; i < MAX_ALIENS; i++)
        aliens[i].active = false;

    int cols = 10;

    for (int i = 0; i < alienCount; i++)
    {
        int row = i / cols;
        int col = i % cols;

        aliens[i].pos = (Vector2){
            100.0f + col * 60.0f,
            80.0f + row * 45.0f
        };

        aliens[i].active = true;
    }

    *alienDirection = 1.0f;
    *alienSpeed = 30.0f + (alienCount - 1) * 0.4f;

    if (*alienSpeed > 50.0f)
        *alienSpeed = 50.0f;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders");
    SetTargetFPS(0);

    int level = LoadLevel();
    int lives = 3;

    Vector2 player = {
        SCREEN_WIDTH / 2.0f - 20.0f,
        SCREEN_HEIGHT - 50.0f
    };

    const float playerSpeed = 300.0f;
    const float alienDrop = 8.0f;

    Alien aliens[MAX_ALIENS];

    float alienDirection = 1.0f;
    float alienSpeed = 30.0f;

    int alienCount = level;

    SetupLevel(
        aliens,
        alienCount,
        &alienSpeed,
        &alienDirection
    );

    Bullet playerBullet = {
        .pos = {0, 0},
        .active = false,
        .speed = 420.0f
    };

    Bullet enemyBullets[MAX_BULLETS] = {0};

    float shootTimer = 0.0f;

    bool gameOver = false;
    bool gameWon = false;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (dt > 0.1f)
            dt = 0.1f;

        if (!gameOver && !gameWon)
        {
            if (IsKeyDown(KEY_A))
                player.x -= playerSpeed * dt;

            if (IsKeyDown(KEY_D))
                player.x += playerSpeed * dt;

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
                playerBullet.pos.y -= playerBullet.speed * dt;

                if (playerBullet.pos.y < -20)
                    playerBullet.active = false;
            }

            bool hitEdge = false;

            for (int i = 0; i < alienCount; i++)
            {
                if (!aliens[i].active)
                    continue;

                float nextX =
                    aliens[i].pos.x +
                    alienDirection * alienSpeed * dt;

                if (nextX < 20.0f ||
                    nextX + 30.0f > SCREEN_WIDTH - 20.0f)
                {
                    hitEdge = true;
                    break;
                }
            }

            if (hitEdge)
            {
                alienDirection *= -1.0f;

                for (int i = 0; i < alienCount; i++)
                {
                    if (aliens[i].active)
                        aliens[i].pos.y += alienDrop;
                }
            }
            else
            {
                for (int i = 0; i < alienCount; i++)
                {
                    if (aliens[i].active)
                    {
                        aliens[i].pos.x +=
                            alienDirection * alienSpeed * dt;
                    }
                }
            }

            if (playerBullet.active)
            {
                Rectangle bulletRect = {
                    playerBullet.pos.x - 2,
                    playerBullet.pos.y - 6,
                    4,
                    12
                };

                for (int i = 0; i < alienCount; i++)
                {
                    if (!aliens[i].active)
                        continue;

                    Rectangle alienRect = {
                        aliens[i].pos.x,
                        aliens[i].pos.y,
                        30,
                        25
                    };

                    if (CheckCollisionRecs(alienRect, bulletRect))
                    {
                        aliens[i].active = false;
                        playerBullet.active = false;
                        break;
                    }
                }
            }

            shootTimer += dt;

            float shootInterval =
                1.2f - (level - 1) * 0.003f;

            if (shootInterval < 1.0f)
                shootInterval = 1.0f;

            if (shootTimer >= shootInterval)
            {
                shootTimer = 0.0f;

                for (int attempts = 0; attempts < 30; attempts++)
                {
                    int index =
                        GetRandomValue(0, alienCount - 1);

                    if (!aliens[index].active)
                        continue;

                    for (int i = 0; i < MAX_BULLETS; i++)
                    {
                        if (!enemyBullets[i].active)
                        {
                            enemyBullets[i].active = true;
                            enemyBullets[i].speed = 180.0f;

                            enemyBullets[i].pos = (Vector2){
                                aliens[index].pos.x + 15,
                                aliens[index].pos.y + 25
                            };

                            break;
                        }
                    }

                    break;
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (!enemyBullets[i].active)
                    continue;

                enemyBullets[i].pos.y +=
                    enemyBullets[i].speed * dt;

                Rectangle enemyBulletRect = {
                    enemyBullets[i].pos.x - 2,
                    enemyBullets[i].pos.y,
                    4,
                    10
                };

                Rectangle playerRect = {
                    player.x,
                    player.y - 10,
                    40,
                    20
                };

                if (CheckCollisionRecs(
                        enemyBulletRect,
                        playerRect))
                {
                    enemyBullets[i].active = false;

                    lives--;

                    if (lives <= 0)
                    {
                        lives = 0;
                        gameOver = true;
                    }

                    break;
                }

                if (enemyBullets[i].pos.y > SCREEN_HEIGHT)
                    enemyBullets[i].active = false;
            }

            bool aliensLeft = false;

            for (int i = 0; i < alienCount; i++)
            {
                if (aliens[i].active)
                {
                    aliensLeft = true;
                    break;
                }
            }

            if (!aliensLeft)
            {
                SaveLevel(level);

                if (level >= MAX_LEVEL)
                {
                    gameWon = true;
                }
                else
                {
                    level++;
                    alienCount = level;

                    SetupLevel(
                        aliens,
                        alienCount,
                        &alienSpeed,
                        &alienDirection
                    );

                    playerBullet.active = false;

                    for (int i = 0; i < MAX_BULLETS; i++)
                        enemyBullets[i].active = false;

                    shootTimer = 0.0f;
                }
            }

            for (int i = 0; i < alienCount; i++)
            {
                if (!aliens[i].active)
                    continue;

                Rectangle alienRect = {
                    aliens[i].pos.x,
                    aliens[i].pos.y,
                    30,
                    25
                };

                Rectangle playerRect = {
                    player.x,
                    player.y - 10,
                    40,
                    20
                };

                if (CheckCollisionRecs(alienRect, playerRect) ||
                    aliens[i].pos.y + 25 >= player.y)
                {
                    lives = 0;
                    gameOver = true;
                    break;
                }
            }
        }
        else
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                if (gameWon)
                {
                    level = 1;
                    SaveLevel(0);
                }

                lives = 3;
                gameOver = false;
                gameWon = false;

                player.x = SCREEN_WIDTH / 2.0f - 20.0f;

                alienCount = level;

                SetupLevel(
                    aliens,
                    alienCount,
                    &alienSpeed,
                    &alienDirection
                );

                playerBullet.active = false;

                for (int i = 0; i < MAX_BULLETS; i++)
                    enemyBullets[i].active = false;

                shootTimer = 0.0f;
            }
        }

        BeginDrawing();

        ClearBackground(BLACK);

        for (int i = 0; i < alienCount; i++)
        {
            if (!aliens[i].active)
                continue;

            Vector2 p = aliens[i].pos;

            DrawRectangle(
                (int)p.x + 5,
                (int)p.y,
                20,
                5,
                GREEN
            );

            DrawRectangle(
                (int)p.x,
                (int)p.y + 5,
                30,
                15,
                GREEN
            );

            DrawRectangle(
                (int)p.x + 5,
                (int)p.y + 20,
                5,
                5,
                GREEN
            );

            DrawRectangle(
                (int)p.x + 20,
                (int)p.y + 20,
                5,
                5,
                GREEN
            );

            DrawRectangle(
                (int)p.x + 7,
                (int)p.y + 7,
                4,
                4,
                BLACK
            );

            DrawRectangle(
                (int)p.x + 19,
                (int)p.y + 7,
                4,
                4,
                BLACK
            );
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

        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!enemyBullets[i].active)
                continue;

            DrawRectangle(
                (int)enemyBullets[i].pos.x - 2,
                (int)enemyBullets[i].pos.y,
                4,
                10,
                WHITE
            );
        }

        DrawText(
            TextFormat("FPS: %d", GetFPS()),
            10,
            10,
            20,
            GREEN
        );

        DrawText(
            TextFormat("LEVEL: %d", level),
            10,
            35,
            20,
            GREEN
        );

        DrawText(
            TextFormat("LIVES: %d", lives),
            SCREEN_WIDTH - 120,
            10,
            20,
            GREEN
        );

        if (gameOver)
        {
            DrawText(
                "GAME OVER",
                SCREEN_WIDTH / 2 - 110,
                SCREEN_HEIGHT / 2 - 40,
                40,
                GREEN
            );

            DrawText(
                "PRESS ENTER TO RESTART",
                SCREEN_WIDTH / 2 - 150,
                SCREEN_HEIGHT / 2 + 20,
                20,
                WHITE
            );
        }

        if (gameWon)
        {
            DrawText(
                "YOU WIN!",
                SCREEN_WIDTH / 2 - 80,
                SCREEN_HEIGHT / 2 - 40,
                40,
                GREEN
            );

            DrawText(
                "PRESS ENTER TO PLAY AGAIN",
                SCREEN_WIDTH / 2 - 155,
                SCREEN_HEIGHT / 2 + 20,
                20,
                WHITE
            );
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}