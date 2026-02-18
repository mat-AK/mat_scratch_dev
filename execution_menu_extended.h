#ifndef UNTITLED7_EXECUTION_MENU_EXTENDED_H
#define UNTITLED7_EXECUTION_MENU_EXTENDED_H

#include "execution_menu.h"
#include "execution_pause.h"

typedef struct {
    ExecutionMenu* base;
    ExecutionPause* pause;

    SDL_Rect runButtonRect;
    SDL_Rect pauseButtonRect;
    SDL_Rect stopButtonRect;
    SDL_Rect stepButtonRect;

    bool isRunning;

} ExtendedExecutionMenu;

ExtendedExecutionMenu* CreateExtendedExecutionMenu(SDL_Renderer* renderer, TTF_Font* font, int x, int y);
void DestroyExtendedExecutionMenu(ExtendedExecutionMenu* menu);
void RenderExtendedExecutionMenu(ExtendedExecutionMenu* menu);
void HandleExtendedExecutionMenuEvent(ExtendedExecutionMenu* menu, SDL_Event* event);
bool ShouldExecuteNextBlock(ExtendedExecutionMenu* menu);
int GetCurrentBlockIndex(ExtendedExecutionMenu* menu);
void BlockExecuted(ExtendedExecutionMenu* menu, int blockIndex);

#endif

