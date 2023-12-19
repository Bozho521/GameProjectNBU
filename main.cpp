#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <SDL2/SDL.h>

const int WIDTH = 750, HEIGHT = 980;
const int MAX_PROJECTILES = 4;
const int MAX_ENEMIES = 5; // Update this value according to your needs

struct Projectile
{
    SDL_Rect rect;
    int speed;
    bool isActive;
    SDL_Texture *texture;
};

struct Enemy
{
    SDL_Rect rect;
    int speed;
    bool isActive;
    SDL_Texture *texture;
};

bool checkCollision(const SDL_Rect &projectileRect, const SDL_Rect &enemyRect)
{
    return SDL_HasIntersection(&projectileRect, &enemyRect);
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Armagedon (imeto)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == nullptr)
    {
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface *planeSurface = SDL_LoadBMP("visuals/plane.bmp");
    SDL_Surface *projectileSurface = SDL_LoadBMP("visuals/projectile.bmp");
    SDL_Surface *enemySurface = SDL_LoadBMP("visuals/enemy.bmp");

    if (planeSurface == nullptr || projectileSurface == nullptr || enemySurface == nullptr)
    {
        std::cout << "Could not load image: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Texture *planeTexture = SDL_CreateTextureFromSurface(renderer, planeSurface);
    SDL_Texture *projectileTexture = SDL_CreateTextureFromSurface(renderer, projectileSurface);
    SDL_Texture *enemyTexture = SDL_CreateTextureFromSurface(renderer, enemySurface);

    SDL_FreeSurface(planeSurface);
    SDL_FreeSurface(projectileSurface);
    SDL_FreeSurface(enemySurface);

    const int NUM_DIGITS = 10;              
    SDL_Texture *digitTextures[NUM_DIGITS];

    for (int i = 0; i < 10; ++i)
    {
        char imagePath[50];
        sprintf(imagePath, "visuals/digit_%d.bmp", i);
        std::cout << "Loading image: " << imagePath << std::endl;

        SDL_Surface *digitSurface = SDL_LoadBMP(imagePath);

        if (digitSurface == nullptr)
        {
            std::cout << "Could not load digit image: " << SDL_GetError() << std::endl;
        }
        else
        {
            digitTextures[i] = SDL_CreateTextureFromSurface(renderer, digitSurface);
            SDL_FreeSurface(digitSurface);
        }
    }

    SDL_SetRenderDrawColor(renderer, 79, 111, 82, 255);

    SDL_ShowCursor(SDL_DISABLE);

    SDL_Event windowEvent;

    Projectile projectiles[MAX_PROJECTILES];

    for (int i = 0; i < MAX_PROJECTILES; ++i)
    {
        projectiles[i].isActive = false;
        projectiles[i].speed = 1;
        projectiles[i].rect.w = 10;
        projectiles[i].rect.h = 10;
        projectiles[i].texture = projectileTexture;
    }

    Enemy enemies[MAX_ENEMIES];

    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        enemies[i].isActive = false;
        enemies[i].speed = 1;                  
        enemies[i].rect.w = rand() % 150 + 50; 
        enemies[i].rect.h = rand() % 150 + 50; 
        enemies[i].texture = enemyTexture;     
    }

    int enemySpawnTimer = 0;             
    const int ENEMY_SPAWN_INTERVAL = 60; 
    const int ENEMY_SPEED_DIVISOR = 2;   

    int score = 0;
    const int ENEMY_SPEED = 1; 

    while (true)
    {
        if (SDL_PollEvent(&windowEvent))
        {
            if (SDL_QUIT == windowEvent.type)
            {
                break;
            }
            if (SDL_MOUSEBUTTONDOWN == windowEvent.type)
            {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                int wingAXOffset = 5;
                int wingBXOffset = 60;
                int wingYOffset = 10;

                if (!projectiles[0].isActive && !projectiles[1].isActive)
                {
                    projectiles[0].isActive = true;
                    projectiles[1].isActive = true;

                    projectiles[0].rect.x = mouseX + wingAXOffset;
                    projectiles[0].rect.y = mouseY + wingYOffset;

                    projectiles[1].rect.x = mouseX + wingBXOffset;
                    projectiles[1].rect.y = mouseY + wingYOffset;
                }
            }
        }

        SDL_RenderClear(renderer);

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_Rect imageRect = {mouseX, mouseY, 75, 75};

        SDL_RenderCopy(renderer, planeTexture, NULL, &imageRect);

        // Enemy spawning logic
        enemySpawnTimer++;
        if (enemySpawnTimer >= ENEMY_SPAWN_INTERVAL)
        {
            enemySpawnTimer = 0;

            for (int i = 0; i < MAX_ENEMIES; ++i)
            {
                if (!enemies[i].isActive)
                {
                    enemies[i].isActive = true;
                    enemies[i].rect.x = rand() % (WIDTH - enemies[i].rect.w); // Random x position within the screen width
                    enemies[i].rect.y = -enemies[i].rect.h;                   // Start above the screen
                    break;
                }
            }
        }

        // Move and render enemies with slower speed
        for (int i = 0; i < MAX_ENEMIES; ++i)
        {
            if (enemies[i].isActive)
            {
                // Move enemies downward with reduced speed
                enemies[i].rect.y += ENEMY_SPEED;

                // Respawn enemies from the top when they go beyond the screen
                if (enemies[i].rect.y > HEIGHT)
                {
                    enemies[i].isActive = false;                              // Deactivate the enemy
                    enemies[i].rect.x = rand() % (WIDTH - enemies[i].rect.w); // Random x position within the screen width
                    enemies[i].rect.y = -enemies[i].rect.h;                   // Start above the screen
                    enemies[i].rect.w = rand() % 150 + 50;                    // Random width within a range
                    enemies[i].rect.h = rand() % 150 + 50;                    // Random height within a range
                    continue;
                }

                // Render enemies
                SDL_RenderCopy(renderer, enemies[i].texture, NULL, &enemies[i].rect); // Render enemies
            }
            else
            {
                // Reactivate inactive enemies when they respawn
                enemies[i].isActive = true;
                enemies[i].rect.x = rand() % (WIDTH - enemies[i].rect.w); // Random x position within the screen width
                enemies[i].rect.y = -enemies[i].rect.h;                   // Start above the screen
                enemies[i].rect.w = rand() % 150 + 50;                    // Random width within a range
                enemies[i].rect.h = rand() % 150 + 50;                    // Random height within a range
            }

            // Check collision with active projectiles
            for (int j = 0; j < MAX_PROJECTILES; ++j)
            {
                if (projectiles[j].isActive)
                {
                    if (checkCollision(projectiles[j].rect, enemies[i].rect))
                    {
                        // Projectile hits the enemy
                        projectiles[j].isActive = false; // Deactivate projectile
                        enemies[i].isActive = false;     // Deactivate enemy
                        score++;                         // Increase the score
                    }
                }
            }
        }

        for (int i = 0; i < MAX_PROJECTILES; ++i)
        {
            if (projectiles[i].isActive)
            {
                projectiles[i].rect.y -= projectiles[i].speed;
                if (projectiles[i].rect.y + projectiles[i].rect.h < 0)
                {
                    projectiles[i].isActive = false;
                }
                else
                {
                    SDL_RenderCopy(renderer, projectiles[i].texture, NULL, &projectiles[i].rect); // Render projectiles
                }
            }
        }

        SDL_Color textColor = {255, 255, 255, 255}; // White color
        int digitWidth = 20;                        // Width of each digit texture
        int digitHeight = 30;                       // Height of each digit texture
        int xPosition = WIDTH - digitWidth;         // Starting x-position for displaying score digits

        // Extract each digit of the score and render the corresponding digit texture
        int remainingScore = score;
        int digitCount = (score == 0) ? 1 : 0; // Calculate the number of digits in the score

        while (remainingScore != 0)
        {
            remainingScore /= 10;
            digitCount++;
        }

        remainingScore = score;

        if (digitCount == 0)
        {
            // Render '0' if the score is 0
            SDL_Rect digitRect = {xPosition, 10, digitWidth, digitHeight};
            SDL_RenderCopy(renderer, digitTextures[0], NULL, &digitRect);
        }
        else
        {
            for (int i = digitCount - 1; i >= 0; --i)
            {
                int digit = remainingScore / static_cast<int>(pow(10, i)) % 10;
                SDL_Rect digitRect = {xPosition, 10, digitWidth, digitHeight};
                SDL_RenderCopy(renderer, digitTextures[digit], NULL, &digitRect);
                xPosition -= digitWidth; // Adjust x-position for the next digit
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(planeTexture);
    SDL_DestroyTexture(projectileTexture);
    SDL_DestroyTexture(enemyTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
