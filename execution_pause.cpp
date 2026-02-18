#include "execution_pause.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ExecutionPause* CreateExecutionPause(SDL_Renderer* renderer, TTF_Font* font) {
    ExecutionPause* pause = (ExecutionPause*)malloc(sizeof(ExecutionPause));
    if (!pause) return NULL;
    pause->isPaused = false;
    pause->isStepMode = false;
    pause->currentBlockIndex = 0;
    pause->lastExecutedIndex = -1;
    pause->showIndicator = true;

    pause->executedColor = (SDL_Color){200, 200, 200, 255};
    pause->pendingColor = (SDL_Color){255, 255, 255, 255};
    pause->indicatorColor = (SDL_Color){0, 120, 255, 255};

    pause->pauseButtonRect.x = 0;
    pause->pauseButtonRect.y = 0;
    pause->pauseButtonRect.w = 40;
    pause->pauseButtonRect.h = 40;

    pause->indicatorRect.w = 5;
    pause->indicatorRect.h = 30;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* playSurf = TTF_RenderUTF8_Blended(font, "▶️", white);
    if (playSurf) {
        pause->playIcon = SDL_CreateTextureFromSurface(renderer, playSurf);
        SDL_FreeSurface(playSurf);
    } else {
        pause->playIcon = NULL;
    }
    SDL_Surface* pauseSurf = TTF_RenderUTF8_Blended(font, "⏸️", white);
    if (pauseSurf) {
        pause->pauseIcon = SDL_CreateTextureFromSurface(renderer, pauseSurf);
        SDL_FreeSurface(pauseSurf);
    } else {
        pause->pauseIcon = NULL;
    }
    SDL_Surface* resumeSurf = TTF_RenderUTF8_Blended(font, "⏯️", white);
    if (resumeSurf) {
        pause->resumeIcon = SDL_CreateTextureFromSurface(renderer, resumeSurf);
        SDL_FreeSurface(resumeSurf);
    } else {
        pause->resumeIcon = NULL;
    }

    printf("✅ ExecutionPause created\n");
    return pause;
}

void DestroyExecutionPause(ExecutionPause* pause) {
    if (pause) {
        if (pause->playIcon) SDL_DestroyTexture(pause->playIcon);
        if (pause->pauseIcon) SDL_DestroyTexture(pause->pauseIcon);
        if (pause->resumeIcon) SDL_DestroyTexture(pause->resumeIcon);
        free(pause);
        printf("✅ ExecutionPause destroyed\n");
    }
}

void TogglePause(ExecutionPause* pause) {
    if (!pause) return;

    pause->isPaused = !pause->isPaused;

    if (pause->isPaused) {
        printf("⏸️ Program PAUSED at block index: %d\n", pause->currentBlockIndex);
    } else {
        printf("▶️ Program RESUMED\n");
        pause->isStepMode = false;
    }
}

void SetPaused(ExecutionPause* pause, bool paused) {
    if (!pause) return;

    if (pause->isPaused != paused) {
        pause->isPaused = paused;
        if (paused) {
            printf("⏸️ Program PAUSED at block index: %d\n", pause->currentBlockIndex);
        } else {
            printf("▶️ Program RESUMED\n");
            pause->isStepMode = false;
        }
    }
}

bool IsPaused(ExecutionPause* pause) {
    return pause ? pause->isPaused : false;
}

void EnterStepMode(ExecutionPause* pause) {
    if (!pause) return;

    pause->isStepMode = true;
    pause->isPaused = true;
    printf("👣 Entered STEP MODE. Press Step to execute one block.\n");
}

void ExitStepMode(ExecutionPause* pause) {
    if (!pause) return;

    pause->isStepMode = false;
    printf("👣 Exited STEP MODE\n");
}

bool IsStepMode(ExecutionPause* pause) {
    return pause ? pause->isStepMode : false;
}

void ExecuteNextStep(ExecutionPause* pause) {
    if (!pause || !pause->isStepMode) return;
    pause->lastExecutedIndex = pause->currentBlockIndex;
    pause->currentBlockIndex++;

    printf("👣 Executed step %d, next block: %d\n",
           pause->lastExecutedIndex, pause->currentBlockIndex);
}

void SetCurrentBlockIndex(ExecutionPause* pause, int index) {
    if (pause) {
        pause->currentBlockIndex = index;
    }
}

void SetLastExecutedIndex(ExecutionPause* pause, int index) {
    if (pause) {
        pause->lastExecutedIndex = index;
    }
}

int GetCurrentBlockIndex(ExecutionPause* pause) {
    return pause ? pause->currentBlockIndex : 0;
}

int GetLastExecutedIndex(ExecutionPause* pause) {
    return pause ? pause->lastExecutedIndex : -1;
}

void RenderPauseIndicator(ExecutionPause* pause, SDL_Renderer* renderer,
                          SDL_Rect* blockRects, int blockCount) {
    if (!pause || !renderer || !blockRects || blockCount == 0) return;
    for (int i = 0; i < blockCount; i++) {
        if (i <= pause->lastExecutedIndex) {
            SDL_SetRenderDrawColor(renderer,
                                   pause->executedColor.r,
                                   pause->executedColor.g,
                                   pause->executedColor.b,
                                   100);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(renderer, &blockRects[i]);
        }
    }
    if (pause->isPaused && pause->currentBlockIndex >= 0 &&
        pause->currentBlockIndex < blockCount) {
        SDL_Rect currentBlock = blockRects[pause->currentBlockIndex];
        pause->indicatorRect.x = currentBlock.x - 10;
        pause->indicatorRect.y = currentBlock.y + (currentBlock.h - 30) / 2;
        SDL_SetRenderDrawColor(renderer,
                               pause->indicatorColor.r,
                               pause->indicatorColor.g,
                               pause->indicatorColor.b,
                               255);
        SDL_RenderFillRect(renderer, &pause->indicatorRect);
    }
}

SDL_Texture* GetCurrentButtonIcon(ExecutionPause* pause) {
    if (!pause) return NULL;

    if (pause->isPaused) {
        return pause->resumeIcon;
    } else {
        return pause->pauseIcon;
    }
}

bool HandlePauseEvent(ExecutionPause* pause, SDL_Event* event, SDL_Rect buttonRect) {
    if (!pause || !event) return false;
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        int x = event->button.x;
        int y = event->button.y;
        if (x >= buttonRect.x && x <= buttonRect.x + buttonRect.w &&
            y >= buttonRect.y && y <= buttonRect.y + buttonRect.h) {
            TogglePause(pause);
            return true;
        }
    }
    return false;
}

void SetPauseButtonPosition(ExecutionPause* pause, int x, int y, int w, int h) {
    if (pause) {
        pause->pauseButtonRect.x = x;
        pause->pauseButtonRect.y = y;
        pause->pauseButtonRect.w = w;
        pause->pauseButtonRect.h = h;
    }
}