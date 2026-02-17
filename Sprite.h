//
// Created by MSI on 2/17/2026.
//

#ifndef UNTITLED74_SPRITE_H
#define UNTITLED74_SPRITE_H
#include <bits/stdc++.h>

using namespace std;

struct Sprite {
    string name;
    float x, y;
    float direction;
    bool isVisible;
    float size;

    bool penDown;
    float stepLimitX, stepLimitY ;
};

#endif //UNTITLED74_SPRITE_H
