#include <iostream>
#include <SDL2/SDL.h>

const int WIDTH = 750, HEIGHT = 980;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Armagedon (imeto)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    if (NULL == window) {
        std::cout << "Could't create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface *imageSurface = SDL_LoadBMP("visuals/character_plane.bmp");
    if (imageSurface == NULL) {
        std::cout << "Could't load image: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    SDL_SetRenderDrawColor(renderer, 79, 111, 82, 255);

    SDL_ShowCursor(SDL_DISABLE); 

    SDL_Event windowEvent;

    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            if (SDL_QUIT == windowEvent.type) {
                break;
            }
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_Rect imageRect = { mouseX, mouseY, 70, 70 };

        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, imageTexture, NULL, &imageRect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(imageTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
