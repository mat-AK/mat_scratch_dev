#include "event_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

EventSystem* CreateEventSystem(void) {
    EventSystem* system = (EventSystem*)malloc(sizeof(EventSystem));
    if (!system) return NULL;
    system->eventQueueHead = 0;
    system->eventQueueTail = 0;
    for (int i = 0; i <= EVENT_BROADCAST_AND_WAIT; i++) {
        system->handlerCount[i] = 0;
        for (int j = 0; j < 32; j++) {
            system->handlers[i][j].spriteId = -1;
            system->handlers[i][j].isActive = true;
            system->handlers[i][j].scriptName[0] = '\0';
            system->handlers[i][j].condition = NULL;
        }
    }
    system->isBroadcasting = false;
    system->currentBroadcast[0] = '\0';
    system->broadcastSender = -1;
    system->waitingHandlers = 0;

    system->timerCount = 0;
    for (int i = 0; i < 16; i++) {
        system->timers[i].active = false;
    }
    system->executeScript = NULL;
    system->executeContext = NULL;

    printf("✅ EventSystem created\n");
    return system;
}

void DestroyEventSystem(EventSystem* system) {
    if (system) {
        free(system);
        printf("✅ EventSystem destroyed\n");
    }
}

void PostEvent(EventSystem* system, GameEvent event) {
    if (!system) return;

    int nextTail = (system->eventQueueTail + 1) % 256;

    if (nextTail != system->eventQueueHead) {
        system->eventQueue[system->eventQueueTail] = event;
        system->eventQueueTail = nextTail;

        printf("📢 Event posted: type=%d\n", event.type);
    } else {
        printf("⚠️ Event queue full!\n");
    }
}

void RegisterEventHandler(EventSystem* system, EventType type,
                          int spriteId, const char* scriptName) {
    if (!system || type < 0 || type > EVENT_BROADCAST_AND_WAIT) return;

    int count = system->handlerCount[type];

    if (count < 32) {
        system->handlers[type][count].spriteId = spriteId;
        strcpy(system->handlers[type][count].scriptName, scriptName);
        system->handlers[type][count].isActive = true;
        system->handlers[type][count].condition = NULL;

        system->handlerCount[type] = count + 1;

        printf("✅ Event handler registered: type=%d, sprite=%d, script=%s\n",
               type, spriteId, scriptName);
    }
}

void UnregisterEventHandler(EventSystem* system, EventType type, int spriteId) {
    if (!system || type < 0 || type > EVENT_BROADCAST_AND_WAIT) return;

    for (int i = 0; i < system->handlerCount[type]; i++) {
        if (system->handlers[type][i].spriteId == spriteId) {
            system->handlers[type][i].isActive = false;
            printf("✅ Event handler unregistered: type=%d, sprite=%d\n", type, spriteId);
            break;
        }
    }
}

void ProcessSDLEvent(EventSystem* system, SDL_Event* sdlEvent) {
    if (!system || !sdlEvent) return;

    GameEvent event;
    event.sdlEvent = *sdlEvent;

    switch (sdlEvent->type) {
        case SDL_QUIT:
            event.type = EVENT_PROGRAM_STOP;
            PostEvent(system, event);
            break;

        case SDL_MOUSEBUTTONDOWN:
            event.type = EVENT_MOUSE_CLICK;
            event.data.mouse.x = sdlEvent->button.x;
            event.data.mouse.y = sdlEvent->button.y;
            event.data.mouse.spriteId = -1;
            PostEvent(system, event);
            break;

        case SDL_KEYDOWN:
            event.type = EVENT_KEY_PRESSED;
            event.data.keyboard.key = sdlEvent->key.keysym.sym;
            event.data.keyboard.keyChar = (char)sdlEvent->key.keysym.sym;
            PostEvent(system, event);
            break;

        default:
            break;
    }
}

void BroadcastMessage(EventSystem* system, const char* message, int senderId) {
    if (!system || !message) return;

    GameEvent event;
    event.type = EVENT_BROADCAST;
    strcpy(event.data.broadcast.message, message);
    event.data.broadcast.senderId = senderId;

    PostEvent(system, event);

    printf("📢 Broadcast: '%s' from sprite %d\n", message, senderId);
}

void BroadcastAndWait(EventSystem* system, const char* message, int senderId) {
    if (!system || !message) return;

    system->isBroadcasting = true;
    strcpy(system->currentBroadcast, message);
    system->broadcastSender = senderId;
    system->waitingHandlers = 0;
    for (int i = 0; i < system->handlerCount[EVENT_BROADCAST]; i++) {
        if (system->handlers[EVENT_BROADCAST][i].isActive) {
            system->waitingHandlers++;
        }
    }

    GameEvent event;
    event.type = EVENT_BROADCAST_AND_WAIT;
    strcpy(event.data.broadcast.message, message);
    event.data.broadcast.senderId = senderId;

    PostEvent(system, event);

    printf("📢 Broadcast and Wait: '%s' from sprite %d (waiting for %d handlers)\n",
           message, senderId, system->waitingHandlers);
}

void ProcessEvents(EventSystem* system) {
    if (!system) return;
    while (system->eventQueueHead != system->eventQueueTail) {
        GameEvent event = system->eventQueue[system->eventQueueHead];
        system->eventQueueHead = (system->eventQueueHead + 1) % 256;

        for (int i = 0; i < system->handlerCount[event.type]; i++) {
            EventHandler* handler = &system->handlers[event.type][i];

            if (!handler->isActive) continue;
            if (handler->condition &&
                !handler->condition(handler->conditionContext)) {
                continue;
            }
            if (system->executeScript) {
                printf("🎬 Executing script '%s' for sprite %d (event type %d)\n",
                       handler->scriptName, handler->spriteId, event.type);

                system->executeScript(handler->scriptName,
                                      handler->spriteId,
                                      system->executeContext);
            }
        }
        if (event.type == EVENT_BROADCAST_AND_WAIT) {}
    }

    Uint32 currentTime = SDL_GetTicks();
    for (int i = 0; i < system->timerCount; i++) {
        if (system->timers[i].active) {
            if (currentTime - system->timers[i].startTime >= system->timers[i].interval) {
                GameEvent timerEvent;
                timerEvent.type = EVENT_TIMER;
                timerEvent.data.timer.elapsedTime = currentTime - system->timers[i].startTime;
                timerEvent.data.timer.timerId = system->timers[i].id;

                PostEvent(system, timerEvent);
                system->timers[i].startTime = currentTime;
            }
        }
    }
}

bool IsKeyPressed(EventSystem* system, SDL_Keycode key) {
    if (!system) return false;

    const Uint8* state = SDL_GetKeyboardState(NULL);
    SDL_Scancode scancode = SDL_GetScancodeFromKey(key);

    return state[scancode] != 0;
}

bool IsMouseButtonPressed(EventSystem* system, int button) {
    if (!system) return false;

    Uint32 buttons = SDL_GetMouseState(NULL, NULL);

    switch(button) {
        case 1: return (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
        case 2: return (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
        case 3: return (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
        default: return false;
    }
}

void GetMousePosition(EventSystem* system, int* x, int* y) {
    if (system) {
        SDL_GetMouseState(x, y);
    }
}

bool IsSpriteClicked(EventSystem* system, int spriteId, SDL_Rect spriteRect) {
    if (!system) return false;
    for (int i = system->eventQueueHead; i != system->eventQueueTail;
         i = (i + 1) % 256) {

        if (system->eventQueue[i].type == EVENT_MOUSE_CLICK) {
            int mx = system->eventQueue[i].data.mouse.x;
            int my = system->eventQueue[i].data.mouse.y;

            if (mx >= spriteRect.x && mx <= spriteRect.x + spriteRect.w &&
                my >= spriteRect.y && my <= spriteRect.y + spriteRect.h) {
                return true;
            }
        }
    }

    return false;
}

int StartTimer(EventSystem* system, Uint32 interval) {
    if (!system || system->timerCount >= 16) return -1;

    int id = system->timerCount;
    system->timers[id].startTime = SDL_GetTicks();
    system->timers[id].interval = interval;
    system->timers[id].active = true;
    system->timers[id].id = id;

    system->timerCount++;

    printf("⏰ Timer %d started with interval %d ms\n", id, interval);
    return id;
}

void StopTimer(EventSystem* system, int timerId) {
    if (system && timerId >= 0 && timerId < system->timerCount) {
        system->timers[timerId].active = false;
        printf("⏰ Timer %d stopped\n", timerId);
    }
}

void SetExecuteCallback(EventSystem* system,
                        void (*callback)(const char*, int, void*),
                        void* context) {
    if (system) {
        system->executeScript = callback;
        system->executeContext = context;
    }
}
