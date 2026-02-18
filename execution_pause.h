#ifndef UNTITLED7_EXECUTION_PAUSE_H
#define UNTITLED7_EXECUTION_PAUSE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct {
    bool isPaused;
    bool isStepMode;
    int currentBlockIndex;
    int lastExecutedIndex;

    SDL_Rect indicatorRect;
    bool showIndicator;

    SDL_Color executedColor;
    SDL_Color pendingColor;
    SDL_Color indicatorColor;
    SDL_Texture* playIcon;
    SDL_Texture* pauseIcon;
    SDL_Texture* resumeIcon;

    SDL_Rect pauseButtonRect;

} ExecutionPause;

ExecutionPause* CreateExecutionPause(SDL_Renderer* renderer, TTF_Font* font);
void DestroyExecutionPause(ExecutionPause* pause);

void TogglePause(ExecutionPause* pause);
void SetPaused(ExecutionPause* pause, bool paused);
bool IsPaused(ExecutionPause* pause);

void EnterStepMode(ExecutionPause* pause);
void ExitStepMode(ExecutionPause* pause);
bool IsStepMode(ExecutionPause* pause);
void ExecuteNextStep(ExecutionPause* pause);

void SetCurrentBlockIndex(ExecutionPause* pause, int index);
void SetLastExecutedIndex(ExecutionPause* pause, int index);
int GetCurrentBlockIndex(ExecutionPause* pause);
int GetLastExecutedIndex(ExecutionPause* pause);

void RenderPauseIndicator(ExecutionPause* pause, SDL_Renderer* renderer,
                          SDL_Rect* blockRects, int blockCount);

SDL_Texture* GetCurrentButtonIcon(ExecutionPause* pause);
bool HandlePauseEvent(ExecutionPause* pause, SDL_Event* event, SDL_Rect buttonRect);
void SetPauseButtonPosition(ExecutionPause* pause, int x, int y, int w, int h);

#endif