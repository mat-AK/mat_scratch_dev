//
// Created by MSI on 2/17/2026.
//

#ifndef UNTITLED74_SCRACHSAFETY_H
#define UNTITLED74_SCRACHSAFETY_H

#include "sprite.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfx.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
void KeepInBounds(Sprite &MySprite , float NumberOfSteps);
void MathSafeguards(int MathX , int MathY);


#endif //UNTITLED74_SCRACHSAFETY_H
