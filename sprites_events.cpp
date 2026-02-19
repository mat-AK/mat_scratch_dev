
#include "sprite_events.h"
#include <stdio.h>
#include <string.h>

void RegisterSpriteEvents(EventSystem* system, EventSprite* sprite) {
    if (!system || !sprite) return;

    for (int i = 0; i < sprite->scriptCount; i++) {
        if (sprite->eventScripts[i].enabled) {
            RegisterEventHandler(system,
                                 sprite->eventScripts[i].eventType,
                                 sprite->id,
                                 sprite->eventScripts[i].scriptName);
        }
    }
}

void CheckSpriteClick(EventSystem* system, EventSprite* sprites, int spriteCount) {
    if (!system || !sprites) return;

    int mouseX, mouseY;
    GetMousePosition(system, &mouseX, &mouseY);

    if (IsMouseButtonPressed(system, 1)) {
        for (int i = 0; i < spriteCount; i++) {
            if (!sprites[i].visible) continue;

            if (mouseX >= sprites[i].rect.x &&
                mouseX <= sprites[i].rect.x + sprites[i].rect.w &&
                mouseY >= sprites[i].rect.y &&
                mouseY <= sprites[i].rect.y + sprites[i].rect.h) {

                GameEvent clickEvent;
                clickEvent.type = EVENT_SPRITE_CLICKED;
                clickEvent.data.mouse.x = mouseX;
                clickEvent.data.mouse.y = mouseY;
                clickEvent.data.mouse.spriteId = sprites[i].id;

                PostEvent(system, clickEvent);
                break;
            }
        }
    }
}

void AttachScriptToSprite(EventSprite* sprite, EventType eventType, const char* scriptName) {
    if (!sprite || sprite->scriptCount >= 16) return;

    sprite->eventScripts[sprite->scriptCount].eventType = eventType;
    strcpy(sprite->eventScripts[sprite->scriptCount].scriptName, scriptName);
    sprite->eventScripts[sprite->scriptCount].enabled = true;
    sprite->scriptCount++;

    printf("✅ Script '%s' attached to sprite %d for event %d\n",
           scriptName, sprite->id, eventType);
}

void DetachScriptFromSprite(EventSprite* sprite, EventType eventType) {
    if (!sprite) return;

    for (int i = 0; i < sprite->scriptCount; i++) {
        if (sprite->eventScripts[i].eventType == eventType) {
            sprite->eventScripts[i].enabled = false;
            printf("✅ Script detached from sprite %d for event %d\n",
                   sprite->id, eventType);
            break;
        }
    }
}

