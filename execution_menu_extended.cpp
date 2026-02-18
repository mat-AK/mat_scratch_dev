#include "execution_menu_extended.h"
#include <stdio.h>
#include <stdlib.h>

ExtendedExecutionMenu* CreateExtendedExecutionMenu(SDL_Renderer* renderer, TTF_Font* font, int x, int y) {
    ExtendedExecutionMenu* menu = (ExtendedExecutionMenu*)malloc(sizeof(ExtendedExecutionMenu));
    if (!menu) return NULL;
    menu->base = CreateExecutionMenu(renderer, font, x, y);
    menu->pause = CreateExecutionPause(renderer, font);
    SDL_Rect bounds = GetExecutionMenuBounds(menu->base);

    menu->runButtonRect = (SDL_Rect){bounds.x, bounds.y, 80, 40};
    menu->pauseButtonRect = (SDL_Rect){bounds.x + 90, bounds.y, 80, 40};
    menu->stopButtonRect = (SDL_Rect){bounds.x + 180, bounds.y, 80, 40};
    menu->stepButtonRect = (SDL_Rect){bounds.x + 270, bounds.y, 80, 40};

    SetPauseButtonPosition(menu->pause,
                           menu->pauseButtonRect.x,
                           menu->pauseButtonRect.y,
                           menu->pauseButtonRect.w,
                           menu->pauseButtonRect.h);

    menu->isRunning = false;

    return menu;
}

void DestroyExtendedExecutionMenu(ExtendedExecutionMenu* menu) {
    if (menu) {
        if (menu->base) DestroyExecutionMenu(menu->base);
        if (menu->pause) DestroyExecutionPause(menu->pause);
        free(menu);
    }
}

void RenderExtendedExecutionMenu(ExtendedExecutionMenu* menu) {
    if (!menu) return;
    RenderExecutionMenu(menu->base);
    SDL_Texture* icon = GetCurrentButtonIcon(menu->pause);
    if (icon) {
    }
}

void HandleExtendedExecutionMenuEvent(ExtendedExecutionMenu* menu, SDL_Event* event) {
    if (!menu || !event) return;
    MenuEvent baseEvent = HandleExecutionMenuEvent(menu->base, event);
    switch(baseEvent) {
        case EVENT_RUN:
            menu->isRunning = true;
            SetPaused(menu->pause, false);
            printf("▶️ Program started\n");
            break;

        case EVENT_PAUSE:
            break;

        case EVENT_STOP:
            menu->isRunning = false;
            SetPaused(menu->pause, false);
            SetCurrentBlockIndex(menu->pause, 0);
            SetLastExecutedIndex(menu->pause, -1);
            printf("⏹️ Program stopped\n");
            break;

        case EVENT_STEP:
            if (IsPaused(menu->pause)) {
                EnterStepMode(menu->pause);
                ExecuteNextStep(menu->pause);
                printf("👣 Step executed\n");
            }
            break;

        default:
            break;
    }
    HandlePauseEvent(menu->pause, event, menu->pauseButtonRect);
}

bool ShouldExecuteNextBlock(ExtendedExecutionMenu* menu) {
    if (!menu || !menu->isRunning) return false;
    if (IsPaused(menu->pause)) {
        if (IsStepMode(menu->pause)) {
            return true;
        }
        return false;
    }
    return true;
}

int GetCurrentBlockIndex(ExtendedExecutionMenu* menu) {
    return menu ? GetCurrentBlockIndex(menu->pause) : 0;
}

void BlockExecuted(ExtendedExecutionMenu* menu, int blockIndex) {
    if (!menu) return;
    SetLastExecutedIndex(menu->pause, blockIndex);
    SetCurrentBlockIndex(menu->pause, blockIndex + 1);

    if (IsStepMode(menu->pause)) {
    }
}
