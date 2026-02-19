#ifndef UNTITLED74_SPRITE_EVENTS_H
#define UNTITLED74_SPRITE_EVENTS_H

#include "event_system.h"
#include <SDL2/SDL.h>

typedef struct {
    int id;
    char name[64];
    SDL_Rect rect;
    bool visible;

    struct {
        EventType eventType;
        char scriptName[64];
        bool enabled;
    } eventScripts[16];
    int scriptCount;

} EventSprite;

void RegisterSpriteEvents(EventSystem* system, EventSprite* sprite);
void CheckSpriteClick(EventSystem* system, EventSprite* sprites, int spriteCount);
void AttachScriptToSprite(EventSprite* sprite, EventType eventType, const char* scriptName);
void DetachScriptFromSprite(EventSprite* sprite, EventType eventType);

#endif