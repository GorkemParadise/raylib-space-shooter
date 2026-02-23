/*******************************************************************************************
*
*   RAYLIB TUTORIAL PROJECT: SPACE SHOOTER GAME
*   =============================================
*
*   This project is designed to teach the basic features of the raylib library.
*   Topics covered:
*     1. Window creation and main loop
*     2. Shape drawing (rectangle, circle, triangle)
*     3. Keyboard and mouse input
*     4. Movement and physics
*     5. Collision detection
*     6. Sound effects and music
*     7. Text and score system
*     8. Particle effects
*     9. Game states (menu, game, game over screen)
*
*   To compile:
*     gcc main.c -o space_shooter -lraylib -lm -lpthread -ldl -lrt -lX11
*
*   Or using CMake:
*     mkdir build && cd build && cmake .. && make
*
********************************************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// =====================================================================
// LESSON 1: CONSTANTS AND STRUCTS
// =====================================================================
// In raylib, we define game objects using structs.
// Each object has a position (Vector2), size, speed, and state.

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600
#define MAX_BULLETS     50
#define MAX_ENEMIES     20
#define MAX_STARS       100
#define MAX_PARTICLES   200
#define MAX_EXPLOSIONS  10

// Game states - menu, game, and game over screen
typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_GAMEOVER
} GameState;

// Player ship
typedef struct {
    Vector2 position;       // x,y position on screen
    Vector2 size;           // Width and height
    float   speed;          // Movement speed (pixels/frame)
    int     health;         // Remaining health
    int     score;          // Total score
    float   shoot_timer;    // Last shot time (for cooldown)
    bool    active;         // Is the player active?
    float   damage_timer;   // For damage animation
} Player;

// Bullet
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float   radius;
    bool    active;
    Color   color;
} Bullet;

// Enemy
typedef struct {
    Vector2 position;
    Vector2 size;
    float   speed;
    int     health;
    bool    active;
    int     type;           // 0: normal, 1: fast, 2: strong
    float   move_angle;     // For wavy movement
} Enemy;

// Star (background)
typedef struct {
    Vector2 position;
    float   speed;
    float   brightness;
    float   size;
} Star;

// Particle effect
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float   radius;
    float   lifetime;       // Remaining lifetime (seconds)
    float   max_lifetime;
    Color   color;
    bool    active;
} Particle;

// =====================================================================
// LESSON 2: GLOBAL VARIABLES
// =====================================================================
// We store all game objects in global arrays.

static Player    player;
static Bullet    bullets[MAX_BULLETS];
static Enemy     enemies[MAX_ENEMIES];
static Star      stars[MAX_STARS];
static Particle  particles[MAX_PARTICLES];
static GameState gameState;
static float     gameTime;
static float     enemyTimer;
static int       wave;              // Enemy wave number
static float     difficultyMultiplier;

// =====================================================================
// LESSON 3: UTILITY FUNCTIONS
// =====================================================================

// Generate a random floating point number
float RandomFloat(float min, float max) {
    return min + (float)GetRandomValue(0, 10000) / 10000.0f * (max - min);
}

// =====================================================================
// LESSON 4: GAME INITIALIZATION
// =====================================================================
// We reset all objects every time a new game starts.

void InitGame(void) {
    // Player initial values
    player.position = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 80.0f };
    player.size = (Vector2){ 40.0f, 40.0f };
    player.speed = 300.0f;
    player.health = 5;
    player.score = 0;
    player.shoot_timer = 0;
    player.active = true;
    player.damage_timer = 0;

    // Deactivate all bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }

    // Deactivate all enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
    }

    // Deactivate all particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = false;
    }

    // LESSON 5: BACKGROUND STARS
    // Parallax effect: stars at different speeds give a sense of depth
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].position = (Vector2){
            (float)GetRandomValue(0, SCREEN_WIDTH),
            (float)GetRandomValue(0, SCREEN_HEIGHT)
        };
        stars[i].speed = RandomFloat(20.0f, 150.0f);
        stars[i].brightness = RandomFloat(0.3f, 1.0f);
        stars[i].size = RandomFloat(1.0f, 3.0f);
    }

    gameTime = 0;
    enemyTimer = 0;
    wave = 1;
    difficultyMultiplier = 1.0f;
}

// =====================================================================
// LESSON 6: PARTICLE SYSTEM
// =====================================================================
// Particle system for explosions and effects.
// Each particle has a lifetime, velocity, and color.

void SpawnParticles(Vector2 position, Color color, int count) {
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++) {
        if (!particles[i].active) {
            particles[i].active = true;
            particles[i].position = position;
            // Spread in random directions
            float angle = RandomFloat(0, 2.0f * PI);
            float spd = RandomFloat(50.0f, 250.0f);
            particles[i].velocity = (Vector2){
                cosf(angle) * spd,
                sinf(angle) * spd
            };
            particles[i].radius = RandomFloat(2.0f, 6.0f);
            particles[i].lifetime = RandomFloat(0.3f, 0.8f);
            particles[i].max_lifetime = particles[i].lifetime;
            particles[i].color = color;
            count--;
        }
    }
}

// =====================================================================
// LESSON 7: SHOOTING BULLETS
// =====================================================================
// Find an empty bullet slot and activate it.

void ShootBullet(Vector2 position, Vector2 velocity, Color color) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].active = true;
            bullets[i].position = position;
            bullets[i].velocity = velocity;
            bullets[i].radius = 4.0f;
            bullets[i].color = color;
            return;
        }
    }
}

// =====================================================================
// LESSON 8: SPAWNING ENEMIES
// =====================================================================
// Different enemy types: normal, fast, strong

void SpawnEnemy(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = true;
            enemies[i].position = (Vector2){
                (float)GetRandomValue(40, SCREEN_WIDTH - 40),
                -40.0f
            };

            // Determine type (harder enemies appear as waves progress)
            int typeChance = GetRandomValue(0, 100);
            if (typeChance < 60) {
                // Normal enemy
                enemies[i].type = 0;
                enemies[i].size = (Vector2){ 30.0f, 30.0f };
                enemies[i].speed = 80.0f + wave * 10.0f;
                enemies[i].health = 1;
            } else if (typeChance < 85) {
                // Fast enemy
                enemies[i].type = 1;
                enemies[i].size = (Vector2){ 20.0f, 20.0f };
                enemies[i].speed = 150.0f + wave * 15.0f;
                enemies[i].health = 1;
            } else {
                // Strong enemy
                enemies[i].type = 2;
                enemies[i].size = (Vector2){ 40.0f, 40.0f };
                enemies[i].speed = 50.0f + wave * 5.0f;
                enemies[i].health = 3;
            }

            enemies[i].move_angle = RandomFloat(0, 2.0f * PI);
            return;
        }
    }
}

// =====================================================================
// LESSON 9: UPDATE - Game Logic
// =====================================================================
// Called every frame. Updates all objects.

void UpdateGame(void) {
    float dt = GetFrameTime(); // Delta time: time between frames
    gameTime += dt;

    // --- PLAYER MOVEMENT ---
    // LESSON: Keyboard input is checked with IsKeyDown()
    if (player.active) {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            player.position.x -= player.speed * dt;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            player.position.x += player.speed * dt;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
            player.position.y -= player.speed * dt;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
            player.position.y += player.speed * dt;

        // Screen boundary clamping
        if (player.position.x < player.size.x / 2)
            player.position.x = player.size.x / 2;
        if (player.position.x > SCREEN_WIDTH - player.size.x / 2)
            player.position.x = SCREEN_WIDTH - player.size.x / 2;
        if (player.position.y < player.size.y / 2)
            player.position.y = player.size.y / 2;
        if (player.position.y > SCREEN_HEIGHT - player.size.y / 2)
            player.position.y = SCREEN_HEIGHT - player.size.y / 2;

        // --- SHOOTING ---
        // LESSON: We limit fire rate using a cooldown system
        player.shoot_timer -= dt;
        if ((IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            && player.shoot_timer <= 0) {
            // Fire double bullets
            ShootBullet(
                (Vector2){ player.position.x - 12, player.position.y - 20 },
                (Vector2){ 0, -500.0f },
                (Color){ 0, 200, 255, 255 }
            );
            ShootBullet(
                (Vector2){ player.position.x + 12, player.position.y - 20 },
                (Vector2){ 0, -500.0f },
                (Color){ 0, 200, 255, 255 }
            );
            player.shoot_timer = 0.15f; // 0.15 second cooldown
        }
    }

    // Update damage animation
    if (player.damage_timer > 0)
        player.damage_timer -= dt;

    // --- UPDATE BULLETS ---
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].position.x += bullets[i].velocity.x * dt;
            bullets[i].position.y += bullets[i].velocity.y * dt;

            // Remove bullets that go off screen
            if (bullets[i].position.y < -10 || bullets[i].position.y > SCREEN_HEIGHT + 10)
                bullets[i].active = false;
        }
    }

    // --- UPDATE ENEMIES ---
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            // Move downward + wavy horizontal movement
            enemies[i].move_angle += dt * 3.0f;
            enemies[i].position.y += enemies[i].speed * dt;
            enemies[i].position.x += sinf(enemies[i].move_angle) * 50.0f * dt;

            // Remove enemies that go off screen
            if (enemies[i].position.y > SCREEN_HEIGHT + 50) {
                enemies[i].active = false;
            }

            // --- COLLISION DETECTION: Bullet vs Enemy ---
            // LESSON: AABB (Axis-Aligned Bounding Box) collision check
            for (int j = 0; j < MAX_BULLETS; j++) {
                if (bullets[j].active && bullets[j].velocity.y < 0) { // Only upward bullets
                    Rectangle enemyRect = {
                        enemies[i].position.x - enemies[i].size.x / 2,
                        enemies[i].position.y - enemies[i].size.y / 2,
                        enemies[i].size.x,
                        enemies[i].size.y
                    };

                    if (CheckCollisionCircleRec(bullets[j].position, bullets[j].radius, enemyRect)) {
                        bullets[j].active = false;
                        enemies[i].health--;

                        if (enemies[i].health <= 0) {
                            enemies[i].active = false;
                            // Explosion effect — unique color per enemy type
                            if (enemies[i].type == 0) {
                                // Normal: red burst
                                SpawnParticles(enemies[i].position, (Color){ 255, 60, 30, 255 }, 10);
                                SpawnParticles(enemies[i].position, (Color){ 255, 160, 50, 255 }, 6);
                            } else if (enemies[i].type == 1) {
                                // Fast: bright cyan/white flash
                                SpawnParticles(enemies[i].position, (Color){ 0, 230, 255, 255 }, 10);
                                SpawnParticles(enemies[i].position, (Color){ 255, 255, 255, 255 }, 5);
                            } else {
                                // Strong: purple + magenta blast
                                SpawnParticles(enemies[i].position, (Color){ 200, 0, 255, 255 }, 15);
                                SpawnParticles(enemies[i].position, (Color){ 255, 80, 200, 255 }, 8);
                            }

                            // Score: different points per type
                            int points[] = { 100, 150, 300 };
                            player.score += points[enemies[i].type];
                        } else {
                            // Took damage but didn't die — light grey sparks
                            SpawnParticles(bullets[j].position, (Color){ 200, 200, 200, 255 }, 4);
                        }
                    }
                }
            }

            // --- COLLISION: Enemy vs Player ---
            if (player.active && player.damage_timer <= 0) {
                Rectangle playerRect = {
                    player.position.x - player.size.x / 2,
                    player.position.y - player.size.y / 2,
                    player.size.x,
                    player.size.y
                };
                Rectangle enemyRect = {
                    enemies[i].position.x - enemies[i].size.x / 2,
                    enemies[i].position.y - enemies[i].size.y / 2,
                    enemies[i].size.x,
                    enemies[i].size.y
                };

                if (CheckCollisionRecs(playerRect, enemyRect)) {
                    enemies[i].active = false;
                    player.health--;
                    player.damage_timer = 1.0f; // 1 second of invincibility
                    // Player hit — blue sparks
                    SpawnParticles(player.position, (Color){ 0, 180, 255, 255 }, 12);
                    SpawnParticles(player.position, (Color){ 255, 255, 255, 255 }, 6);

                    if (player.health <= 0) {
                        player.active = false;
                        // Player death — big multi-color explosion
                        SpawnParticles(player.position, (Color){ 0, 180, 255, 255 }, 30);
                        SpawnParticles(player.position, (Color){ 255, 255, 255, 255 }, 20);
                        SpawnParticles(player.position, (Color){ 100, 220, 255, 255 }, 15);
                        gameState = STATE_GAMEOVER;
                    }
                }
            }
        }
    }

    // --- UPDATE PARTICLES ---
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].position.x += particles[i].velocity.x * dt;
            particles[i].position.y += particles[i].velocity.y * dt;
            particles[i].lifetime -= dt;
            // Drag effect
            particles[i].velocity.x *= 0.98f;
            particles[i].velocity.y *= 0.98f;

            if (particles[i].lifetime <= 0)
                particles[i].active = false;
        }
    }

    // --- UPDATE STARS (Parallax) ---
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].position.y += stars[i].speed * dt;
        if (stars[i].position.y > SCREEN_HEIGHT) {
            stars[i].position.y = 0;
            stars[i].position.x = (float)GetRandomValue(0, SCREEN_WIDTH);
        }
    }

    // --- ENEMY WAVE SYSTEM ---
    enemyTimer += dt;
    float spawnInterval = 2.0f / difficultyMultiplier; // More frequent as difficulty increases
    if (enemyTimer >= spawnInterval) {
        enemyTimer = 0;
        SpawnEnemy();
    }

    // Difficulty increases every 30 seconds
    difficultyMultiplier = 1.0f + gameTime / 30.0f;
    wave = 1 + (int)(gameTime / 20.0f);
}

// =====================================================================
// LESSON 10: DRAWING FUNCTIONS
// =====================================================================
// In raylib, drawing is done between BeginDrawing() and EndDrawing().

// Draw the player ship (ship shape made of triangles)
void DrawPlayer(void) {
    if (!player.active) return;

    // Flash when damaged
    if (player.damage_timer > 0 && (int)(player.damage_timer * 10) % 2 == 0)
        return;

    float x = player.position.x;
    float y = player.position.y;

    // Ship body (triangle)
    DrawTriangle(
        (Vector2){ x, y - 22 },          // Top tip (nose)
        (Vector2){ x - 18, y + 15 },     // Bottom left
        (Vector2){ x + 18, y + 15 },     // Bottom right
        (Color){ 50, 150, 255, 255 }
    );

    // Inner detail
    DrawTriangle(
        (Vector2){ x, y - 14 },
        (Vector2){ x - 10, y + 8 },
        (Vector2){ x + 10, y + 8 },
        (Color){ 100, 200, 255, 255 }
    );

    // Wings
    DrawTriangle(
        (Vector2){ x - 18, y + 15 },
        (Vector2){ x - 28, y + 22 },
        (Vector2){ x - 8, y + 10 },
        (Color){ 30, 100, 200, 255 }
    );
    DrawTriangle(
        (Vector2){ x + 18, y + 15 },
        (Vector2){ x + 28, y + 22 },
        (Vector2){ x + 8, y + 10 },
        (Color){ 30, 100, 200, 255 }
    );

    // Engine flame (animated)
    float flame = sinf(GetTime() * 20.0f) * 5.0f;
    DrawTriangle(
        (Vector2){ x - 6, y + 15 },
        (Vector2){ x, y + 28 + flame },
        (Vector2){ x + 6, y + 15 },
        (Color){ 255, 150, 0, 200 }
    );
    DrawTriangle(
        (Vector2){ x - 3, y + 15 },
        (Vector2){ x, y + 22 + flame },
        (Vector2){ x + 3, y + 15 },
        YELLOW
    );
}

// Draw enemy (different shape based on type)
void DrawEnemy(Enemy *e) {
    float x = e->position.x;
    float y = e->position.y;

    if (e->type == 0) {
        // Normal enemy: Deep red-purple square
        DrawRectanglePro(
            (Rectangle){ x, y, e->size.x, e->size.y },
            (Vector2){ e->size.x / 2, e->size.y / 2 },
            sinf(e->move_angle) * 15.0f,
            (Color){ 180, 20, 80, 255 }
        );
        DrawRectanglePro(
            (Rectangle){ x, y, e->size.x * 0.6f, e->size.y * 0.6f },
            (Vector2){ e->size.x * 0.3f, e->size.y * 0.3f },
            sinf(e->move_angle) * 15.0f,
            (Color){ 240, 60, 130, 255 }
        );
    } else if (e->type == 1) {
        // Fast enemy: Magenta — drawn as filled polygon using lines to avoid winding issues
        float hw = 14.0f;  // half-width
        float hh = 13.0f;  // half-height
        // Draw as two overlapping rects to fake a diamond/triangle shape
        // Top triangle: v0 top, v1 bottom-left, v2 bottom-right (clockwise for raylib screen coords)
        Vector2 v0 = { x,       y - hh }; // tip top
        Vector2 v1 = { x + hw,  y + hh }; // bottom right
        Vector2 v2 = { x - hw,  y + hh }; // bottom left
        DrawTriangle(v0, v1, v2, (Color){ 220, 0, 120, 255 });

        // Inner highlight
        Vector2 i0 = { x,      y - 6.0f };
        Vector2 i1 = { x + 7,  y + 6.0f };
        Vector2 i2 = { x - 7,  y + 6.0f };
        DrawTriangle(i0, i1, i2, (Color){ 255, 80, 180, 255 });

        // Bright outline so it's always visible
        DrawTriangleLines(v0, v1, v2, (Color){ 255, 150, 220, 255 });
    } else {
        // Strong enemy: Bright purple hexagon
        DrawPoly((Vector2){ x, y }, 6, e->size.x / 2, e->move_angle * 10, (Color){ 140, 0, 200, 255 });
        DrawPoly((Vector2){ x, y }, 6, e->size.x / 3, e->move_angle * 10, (Color){ 200, 60, 255, 255 });
        // Health indicator
        for (int c = 0; c < e->health; c++) {
            DrawCircle((int)(x - 8 + c * 8), (int)(y - e->size.y / 2 - 8), 3, (Color){ 255, 80, 180, 255 });
        }
    }
}

// =====================================================================
// LESSON 11: MAIN DRAW FUNCTION
// =====================================================================

void DrawGame(void) {
    // Background: Dark space
    ClearBackground((Color){ 5, 5, 20, 255 });

    // Stars
    for (int i = 0; i < MAX_STARS; i++) {
        float alpha = stars[i].brightness * 255;
        Color starColor = (Color){ 200, 200, 255, (unsigned char)alpha };
        DrawCircleV(stars[i].position, stars[i].size, starColor);
    }

    // Bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // Bullet glow effect
            DrawCircleV(bullets[i].position, bullets[i].radius * 3,
                       Fade(bullets[i].color, 0.15f));
            DrawCircleV(bullets[i].position, bullets[i].radius * 1.5f,
                       Fade(bullets[i].color, 0.4f));
            DrawCircleV(bullets[i].position, bullets[i].radius, bullets[i].color);
        }
    }

    // Enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            DrawEnemy(&enemies[i]);
        }
    }

    // Player
    DrawPlayer();

    // Particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            float ratio = particles[i].lifetime / particles[i].max_lifetime;
            float r = particles[i].radius * ratio;
            Color color = particles[i].color;
            color.a = (unsigned char)(255 * ratio);
            DrawCircleV(particles[i].position, r * 2, Fade(color, 0.2f));
            DrawCircleV(particles[i].position, r, color);
        }
    }

    // --- HUD (Heads-Up Display) ---
    // Health indicator
    DrawText("HP:", 10, 10, 20, WHITE);
    for (int i = 0; i < player.health; i++) {
        DrawRectangle(50 + i * 25, 12, 18, 18, (Color){ 255, 50, 50, 255 });
        DrawRectangleLines(50 + i * 25, 12, 18, 18, WHITE);
    }

    // Score
    char scoreText[64];
    snprintf(scoreText, sizeof(scoreText), "SCORE: %d", player.score);
    DrawText(scoreText, SCREEN_WIDTH - 200, 10, 20, (Color){ 0, 255, 200, 255 });

    // Wave info
    char waveText[32];
    snprintf(waveText, sizeof(waveText), "WAVE: %d", wave);
    DrawText(waveText, SCREEN_WIDTH / 2 - 40, 10, 20, YELLOW);

    // Time
    char timeText[32];
    snprintf(timeText, sizeof(timeText), "%.1f sec", gameTime);
    DrawText(timeText, SCREEN_WIDTH - 80, 35, 16, GRAY);
}

// =====================================================================
// LESSON 12: MENU SCREEN
// =====================================================================

void DrawMenu(void) {
    ClearBackground((Color){ 5, 5, 20, 255 });

    // Star background is also active in the menu
    for (int i = 0; i < MAX_STARS; i++) {
        float dt = GetFrameTime();
        stars[i].position.y += stars[i].speed * dt;
        if (stars[i].position.y > SCREEN_HEIGHT) {
            stars[i].position.y = 0;
            stars[i].position.x = (float)GetRandomValue(0, SCREEN_WIDTH);
        }
        float alpha = stars[i].brightness * 255;
        DrawCircleV(stars[i].position, stars[i].size,
                   (Color){ 200, 200, 255, (unsigned char)alpha });
    }

    // Title (animated)
    float titleY = 120 + sinf(GetTime() * 2.0f) * 10.0f;
    const char *title = "SPACE SHOOTER";
    int titleWidth = MeasureText(title, 50);
    DrawText(title, SCREEN_WIDTH / 2 - titleWidth / 2 + 2, (int)titleY + 2, 50, DARKBLUE);
    DrawText(title, SCREEN_WIDTH / 2 - titleWidth / 2, (int)titleY, 50, (Color){ 0, 200, 255, 255 });

    // Subtitle
    const char *sub = "made with raylib";
    int subWidth = MeasureText(sub, 20);
    DrawText(sub, SCREEN_WIDTH / 2 - subWidth / 2, (int)titleY + 60, 20, GRAY);

    // Start button (blinking)
    float alpha = (sinf(GetTime() * 3.0f) + 1.0f) / 2.0f;
    Color buttonColor = { 0, 200, 255, (unsigned char)(150 + alpha * 105) };
    const char *start = "[ ENTER ] to START";
    int startWidth = MeasureText(start, 24);
    DrawText(start, SCREEN_WIDTH / 2 - startWidth / 2, 320, 24, buttonColor);

    // Controls info
    int infoY = 420;
    DrawText("CONTROLS:", SCREEN_WIDTH / 2 - 80, infoY, 20, WHITE);
    DrawText("WASD / Arrow Keys  -  Move", SCREEN_WIDTH / 2 - 140, infoY + 35, 16, LIGHTGRAY);
    DrawText("SPACE / Left Click -  Shoot", SCREEN_WIDTH / 2 - 140, infoY + 60, 16, LIGHTGRAY);

    // Enemy type info
    DrawRectangle(SCREEN_WIDTH / 2 - 120, infoY + 100, 18, 18, (Color){ 180, 20, 80, 255 });
    DrawText("Normal (100 pts)", SCREEN_WIDTH / 2 - 90, infoY + 100, 16, LIGHTGRAY);

    // Fast enemy preview (tip pointing down, clockwise)
    {
        float mx = SCREEN_WIDTH / 2.0f - 111;
        float my = infoY + 135.0f;
        Vector2 mv0 = { mx,      my - 10 };
        Vector2 mv1 = { mx + 10, my + 8  };
        Vector2 mv2 = { mx - 10, my + 8  };
        DrawTriangle(mv0, mv1, mv2, (Color){ 220, 0, 120, 255 });
        DrawTriangleLines(mv0, mv1, mv2, (Color){ 255, 150, 220, 255 });
    }
    DrawText("Fast   (150 pts)", SCREEN_WIDTH / 2 - 90, infoY + 128, 16, LIGHTGRAY);

    DrawPoly((Vector2){ SCREEN_WIDTH / 2.0f - 111, infoY + 165.0f }, 6, 10, 0, (Color){ 140, 0, 200, 255 });
    DrawText("Strong (300 pts)", SCREEN_WIDTH / 2 - 90, infoY + 156, 16, LIGHTGRAY);

    if (IsKeyPressed(KEY_ENTER)) {
        InitGame();
        gameState = STATE_GAME;
    }
}

// =====================================================================
// LESSON 13: GAME OVER SCREEN
// =====================================================================

void DrawGameOver(void) {
    ClearBackground((Color){ 5, 5, 20, 255 });

    // Keep stars going
    for (int i = 0; i < MAX_STARS; i++) {
        float dt = GetFrameTime();
        stars[i].position.y += stars[i].speed * dt * 0.3f;
        if (stars[i].position.y > SCREEN_HEIGHT) {
            stars[i].position.y = 0;
        }
        float alpha = stars[i].brightness * 200;
        DrawCircleV(stars[i].position, stars[i].size,
                   (Color){ 200, 200, 255, (unsigned char)alpha });
    }

    // Keep particles going
    float dt = GetFrameTime();
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].position.x += particles[i].velocity.x * dt;
            particles[i].position.y += particles[i].velocity.y * dt;
            particles[i].lifetime -= dt;
            particles[i].velocity.x *= 0.98f;
            particles[i].velocity.y *= 0.98f;
            if (particles[i].lifetime <= 0) particles[i].active = false;

            float ratio = particles[i].lifetime / particles[i].max_lifetime;
            Color color = particles[i].color;
            color.a = (unsigned char)(255 * ratio);
            DrawCircleV(particles[i].position, particles[i].radius * ratio, color);
        }
    }

    // Title
    const char *gameOver = "GAME OVER!";
    int gameOverWidth = MeasureText(gameOver, 50);
    DrawText(gameOver, SCREEN_WIDTH / 2 - gameOverWidth / 2 + 2, 152, 50, MAROON);
    DrawText(gameOver, SCREEN_WIDTH / 2 - gameOverWidth / 2, 150, 50, RED);

    // Score
    char scoreText[64];
    snprintf(scoreText, sizeof(scoreText), "SCORE: %d", player.score);
    int scoreWidth = MeasureText(scoreText, 36);
    DrawText(scoreText, SCREEN_WIDTH / 2 - scoreWidth / 2, 230, 36, (Color){ 0, 255, 200, 255 });

    // Stats
    char timeText[64];
    snprintf(timeText, sizeof(timeText), "Survival time: %.1f seconds", gameTime);
    int timeWidth = MeasureText(timeText, 20);
    DrawText(timeText, SCREEN_WIDTH / 2 - timeWidth / 2, 285, 20, LIGHTGRAY);

    char waveText[64];
    snprintf(waveText, sizeof(waveText), "Wave reached: %d", wave);
    int waveWidth = MeasureText(waveText, 20);
    DrawText(waveText, SCREEN_WIDTH / 2 - waveWidth / 2, 315, 20, LIGHTGRAY);

    // Play again
    float alpha = (sinf(GetTime() * 3.0f) + 1.0f) / 2.0f;
    Color buttonColor = { 255, 200, 0, (unsigned char)(150 + alpha * 105) };
    const char *retry = "[ ENTER ] to PLAY AGAIN";
    int retryWidth = MeasureText(retry, 24);
    DrawText(retry, SCREEN_WIDTH / 2 - retryWidth / 2, 400, 24, buttonColor);

    const char *menu = "[ ESC ] for MENU";
    int menuWidth = MeasureText(menu, 20);
    DrawText(menu, SCREEN_WIDTH / 2 - menuWidth / 2, 440, 20, GRAY);

    if (IsKeyPressed(KEY_ENTER)) {
        InitGame();
        gameState = STATE_GAME;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        gameState = STATE_MENU;
    }
}

// =====================================================================
// LESSON 14: MAIN FUNCTION (Main Loop)
// =====================================================================
// The basic raylib structure:
//   1. InitWindow()  - Create the window
//   2. while loop    - Game loop (update + draw)
//   3. CloseWindow() - Clean up and close

int main(void) {
    // --- Window creation ---
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Shooter - raylib Tutorial Project");
    SetTargetFPS(60);   // Target frame rate

    // Initial state
    gameState = STATE_MENU;
    InitGame(); // Initialize stars

    // =====================================================
    // MAIN GAME LOOP
    // =====================================================
    // WindowShouldClose() returns true when the window is closed
    while (!WindowShouldClose()) {
        // --- Update phase ---
        switch (gameState) {
            case STATE_MENU:
                // Menu input is handled inside DrawMenu
                break;
            case STATE_GAME:
                UpdateGame();
                if (IsKeyPressed(KEY_ESCAPE)) gameState = STATE_MENU;
                break;
            case STATE_GAMEOVER:
                // Game over input is handled inside DrawGameOver
                break;
        }

        // --- Draw phase ---
        BeginDrawing();
        switch (gameState) {
            case STATE_MENU:     DrawMenu();     break;
            case STATE_GAME:     DrawGame();     break;
            case STATE_GAMEOVER: DrawGameOver(); break;
        }
        EndDrawing();
    }

    // --- Cleanup ---
    CloseWindow();
    return 0;
}