#ifndef UNTITLED7_EXECUTION_MENU_H
#define UNTITLED7_EXECUTION_MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <functional>

class ExecutionMenu {
public:
    enum ExecutionState {
        STATE_STOPPED,
        STATE_RUNNING,
        STATE_PAUSED,
        STATE_STEPPING
    };

    enum MenuEvent {
        EVENT_NONE,
        EVENT_RUN,
        EVENT_PAUSE,
        EVENT_STOP,
        EVENT_STEP,
        EVENT_RESUME
    };

    ExecutionMenu(SDL_Renderer* renderer, TTF_Font* font, int x, int y);

    ~ExecutionMenu();
    void render();
    MenuEvent handleEvent(SDL_Event& event);
    void updateState(ExecutionState newState);
    ExecutionState getCurrentState() const;

    void setRunCallback(std::function<void()> callback);
    void setPauseCallback(std::function<void()> callback);
    void setStopCallback(std::function<void()> callback);
    void setStepCallback(std::function<void()> callback);
    void setPosition(int x, int y);
    SDL_Rect getBounds() const;

private:
    SDL_Renderer* renderer;
    TTF_Font* font;

    SDL_Rect menuRect;
    int buttonWidth;
    int buttonHeight;
    int buttonSpacing;

    SDL_Rect runButton, pauseButton, stopButton, stepButton;
    bool runHovered, pauseHovered, stopHovered, stepHovered;
    ExecutionState currentState;
    std::function<void()> onRun;
    std::function<void()> onPause;
    std::function<void()> onStop;
    std::function<void()> onStep;

    void renderButton(SDL_Rect rect, const std::string& text,
                      bool hovered, bool enabled);

    SDL_Color getButtonColor(const std::string& buttonType,
                             bool hovered, bool enabled);

    void renderText(const std::string& text, int x, int y,
                    SDL_Color color);

    bool isPointInRect(int x, int y, SDL_Rect rect);

    SDL_Texture* createTextTexture(const std::string& text,
                                   SDL_Color color);
};

#endif
