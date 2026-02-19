//
// Created by MSI on 2/17/2026.
//
#include "ScrachSafety.h"

void StringWriter(const char * text){
    Uint32 SDL_flags = SDL_INIT_VIDEO | SDL_INIT_TIMER ;
    Uint32 WND_flags =  SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP;

    SDL_Window *m_window;
    SDL_Renderer *m_renderer;

    SDL_Init(SDL_flags);
    SDL_CreateWindowAndRenderer(1920 , 480 , WND_flags , &m_window, &m_renderer);

    SDL_RaiseWindow(m_window);

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0 , &DM);

}

void KeepInBounds(Sprite &MySprite , float NumberOfSteps)
{

    if ( NumberOfSteps + MySprite.x > MySprite.stepLimitX)
        MySprite.x=MySprite.stepLimitX;
    if ( NumberOfSteps + MySprite.y > MySprite.stepLimitY)
        MySprite.y=MySprite.stepLimitY;
}

void MathSafeguards(int MathX , int MathY)
{
    if (MathY==0)
    {
        const char * textME ="Math Error";
        StringWriter(textME);
    }
}


