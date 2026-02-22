#include "variables.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

static void renderText(SDL_Renderer* renderer, const string& text,int x, int y, SDL_Color color, TTF_Font* font)
{
    if (!font || text.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cerr << "SDL initialization failed: " << SDL_GetError() << endl;
        return 1;
    }

    if (TTF_Init() < 0)
    {
        cerr << "SDL_ttf initialization failed: " << TTF_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Variable System",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          1000, 750,
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        cerr << "Window creation failed: " << SDL_GetError() << endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        cerr << "Renderer creation failed: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", 14);
    TTF_Font* titleFont = TTF_OpenFont("arial.ttf", 18);
    if (!font)
    {
        cerr << "Font loading failed: " << TTF_GetError() << endl;
        cerr << "Continuing without font - variables won't render" << endl;
    }

    VariableManager vm;
    var_init(vm);
    var_set_fonts(vm, font, titleFont);

    var_toggle_panel(vm);

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_v)
                    {
                        var_toggle_panel(vm);
                    }
                    else if (event.key.keysym.sym == SDLK_b)
                    {
                        var_toggle_blocks_panel(vm);
                    }
                    else if (event.key.keysym.sym == SDLK_t)
                    {
                        var_toggle_table_view(vm);
                    }
                    else if (event.key.keysym.sym == SDLK_n)
                    {
                        var_show_create_dialog(vm, true);
                    }
                    else if (event.key.keysym.sym == SDLK_SPACE)
                    {
                        var_execute_all_blocks(vm);
                    }
                    else
                    {
                        var_handle_keyboard(vm, event.key.keysym);
                    }
                    break;

                case SDL_TEXTINPUT:
                    var_handle_text_input(vm, event.text.text);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        var_handle_mouse_button(vm, event.button.x, event.button.y, true);
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        var_handle_mouse_release(vm);
                    }
                    break;

                case SDL_MOUSEMOTION:
                    var_handle_mouse_motion(vm, event.motion.x, event.motion.y);
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        if (font)
        {
            renderText(renderer, "=== Variable System Controls ===", 350, 10,
                       {200, 200, 200, 255}, titleFont);
            renderText(renderer, "V: Toggle variables panel", 350, 35,
                       {180, 180, 180, 255}, font);
            renderText(renderer, "B: Toggle blocks panel", 350, 55,
                       {180, 180, 180, 255}, font);
            renderText(renderer, "T: Toggle table view", 350, 75,
                       {180, 180, 180, 255}, font);
            renderText(renderer, "N: Create new variable", 350, 95,
                       {180, 180, 180, 255}, font);
            renderText(renderer, "SPACE: Execute all blocks", 350, 115,
                       {180, 180, 180, 255}, font);

            renderText(renderer, "=== Current Variables ===", 350, 145,
                       {200, 200, 200, 255}, titleFont);
        }

        var_render(renderer, vm);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (font) TTF_CloseFont(font);
    if (titleFont) TTF_CloseFont(titleFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}