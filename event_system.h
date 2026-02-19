#ifndef UNTITLED74_EVENT_SYSTEM_H
#define UNTITLED74_EVENT_SYSTEM_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef enum {
    EVENT_NONE,
    EVENT_PROGRAM_START,
    EVENT_PROGRAM_STOP,
    EVENT_MOUSE_CLICK,
    EVENT_SPRITE_CLICKED,
    EVENT_KEY_PRESSED,
    EVENT_BROADCAST,
    EVENT_BROADCAST_AND_WAIT,
    EVENT_TIMER,

} EventType;

typedef struct {
    EventType type;
    SDL_Event sdlEvent;

    union {
        struct {
            int x, y;
            int spriteId;
        } mouse;

        struct {
            SDL_Keycode key;
            char keyChar;
        } keyboard;

        struct {
            char message[256];
            int senderId;
        } broadcast;

        struct {
            Uint32 elapsedTime;
            int timerId;
        } timer;
    } data;

} GameEvent;

typedef struct {
    int spriteId;
    char scriptName[64];
    bool isActive;
    bool (*condition)(void* context);
    void* conditionContext;

} EventHandler;

typedef struct EventSystem {
    GameEvent eventQueue[256];
    int eventQueueHead;
    int eventQueueTail;
    EventHandler handlers[EVENT_BROADCAST_AND_WAIT + 1][32];
    int handlerCount[EVENT_BROADCAST_AND_WAIT + 1];

    bool isBroadcasting;
    char currentBroadcast[256];
    int broadcastSender;
    int waitingHandlers;

    struct {
        Uint32 startTime;
        Uint32 interval;
        bool active;
        int id;
    } timers[16];
    int timerCount;
    void (*executeScript)(const char* scriptName, int spriteId, void* context);
    void* executeContext;

} EventSystem;

EventSystem* CreateEventSystem(void);
void DestroyEventSystem(EventSystem* system);

void RegisterEventHandler(EventSystem* system, EventType type,
                          int spriteId, const char* scriptName);
void UnregisterEventHandler(EventSystem* system, EventType type, int spriteId);
void PostEvent(EventSystem* system, GameEvent event);
void BroadcastMessage(EventSystem* system, const char* message, int senderId);
void BroadcastAndWait(EventSystem* system, const char* message, int senderId);
void ProcessEvents(EventSystem* system);
void ProcessSDLEvent(EventSystem* system, SDL_Event* sdlEvent);
bool IsKeyPressed(EventSystem* system, SDL_Keycode key);
bool IsMouseButtonPressed(EventSystem* system, int button);
void GetMousePosition(EventSystem* system, int* x, int* y);
bool IsSpriteClicked(EventSystem* system, int spriteId, SDL_Rect spriteRect);

int StartTimer(EventSystem* system, Uint32 interval);
void StopTimer(EventSystem* system, int timerId);

void SetExecuteCallback(EventSystem* system,
                        void (*callback)(const char*, int, void*),
                        void* context);

#endif
