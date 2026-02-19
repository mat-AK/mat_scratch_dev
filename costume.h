//
// Created by MSI on 2/19/2026.
//

#ifndef UNTITLED75_COSTUME_H
#define UNTITLED75_COSTUME_H
#include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfx.h>


using namespace std;

// ============= Constants =============
#define MAX_COSTUMES 50
#define MAX_NAME_LEN 50
#define MAX_PATH_LEN 256
#define MAX_FRAMES 100
#define MAX_RECORDINGS 20

// ============= Costume (Image) Structure =============
struct Costume {
    char name[MAX_NAME_LEN];
    char filePath[MAX_PATH_LEN];
    SDL_Texture* texture;
    int width;
    int height;
    bool isLoaded;

    Costume() {
        name[0] = '\0';
        filePath[0] = '\0';
        texture = nullptr;
        width = 0;
        height = 0;
        isLoaded = false;
    }

    void setName(const char* n) {
        strcpy(name, n);
    }

    void setPath(const char* path) {
        strcpy(filePath, path);
    }

    bool load(SDL_Renderer* renderer) {
        if (!renderer) return false;

        // Load image using SDL_image (simplified)
        // In real code, use IMG_LoadTexture
        SDL_Surface* surface = SDL_LoadBMP(filePath);
        if (!surface) {
            cout << "Failed to load image: " << filePath << endl;
            return false;
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);
        width = surface->w;
        height = surface->h;
        SDL_FreeSurface(surface);

        if (texture) {
            isLoaded = true;
            cout << "Loaded costume: " << name << " (" << width << "x" << height << ")" << endl;
        }

        return isLoaded;
    }

    void unload() {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
        isLoaded = false;
    }

    void render(SDL_Renderer* renderer, int x, int y) {
        if (!isLoaded || !texture) return;

        SDL_Rect dest = {x, y, width, height};
        SDL_RenderCopy(renderer, texture, nullptr, &dest);
    }

    void renderScaled(SDL_Renderer* renderer, int x, int y, int w, int h) {
        if (!isLoaded || !texture) return;

        SDL_Rect dest = {x, y, w, h};
        SDL_RenderCopy(renderer, texture, nullptr, &dest);
    }
};

// ============= Frame Structure (for animation) =============
struct Frame {
    int x, y;           // position
    int w, h;           // size
    int duration;       // milliseconds
    int costumeIndex;   // which costume to use

    Frame() {
        x = 0; y = 0;
        w = 100; h = 100;
        duration = 100;
        costumeIndex = -1;
    }

    void setPosition(int px, int py) {
        x = px;
        y = py;
    }

    void setSize(int pw, int ph) {
        w = pw;
        h = ph;
    }
};

// ============= Animation Recording =============
struct Animation {
    char name[MAX_NAME_LEN];
    Frame frames[MAX_FRAMES];
    int frameCount;
    bool isLooping;
    int currentFrame;
    Uint32 lastFrameTime;

    Animation() {
        name[0] = '\0';
        frameCount = 0;
        isLooping = true;
        currentFrame = 0;
        lastFrameTime = 0;
    }

    void setName(const char* n) {
        strcpy(name, n);
    }

    void addFrame(int x, int y, int costumeIdx) {
        if (frameCount < MAX_FRAMES) {
            frames[frameCount].setPosition(x, y);
            frames[frameCount].costumeIndex = costumeIdx;
            frameCount++;
        }
    }

    void addFrameWithSize(int x, int y, int w, int h, int costumeIdx) {
        if (frameCount < MAX_FRAMES) {
            frames[frameCount].setPosition(x, y);
            frames[frameCount].setSize(w, h);
            frames[frameCount].costumeIndex = costumeIdx;
            frameCount++;
        }
    }

    void start() {
        currentFrame = 0;
        lastFrameTime = SDL_GetTicks();
    }

    Frame* getCurrentFrame() {
        if (frameCount == 0) return nullptr;
        return &frames[currentFrame];
    }

    void update() {
        if (frameCount == 0) return;

        Uint32 now = SDL_GetTicks();
        if (now - lastFrameTime >= frames[currentFrame].duration) {
            // Move to next frame
            currentFrame++;
            if (currentFrame >= frameCount) {
                if (isLooping) {
                    currentFrame = 0;  // loop back
                } else {
                    currentFrame = frameCount - 1;  // stay at last frame
                }
            }
            lastFrameTime = now;
        }
    }

    void print() const {
        cout << "Animation: " << name << " (" << frameCount << " frames)" << endl;
        for (int i = 0; i < frameCount; i++) {
            cout << "  Frame " << i << ": (" << frames[i].x << "," << frames[i].y
                 << ") size=" << frames[i].w << "x" << frames[i].h << endl;
        }
    }
};

// ============= Recording Session =============
struct RecordingSession {
    Animation* targetAnim;
    bool isRecording;
    int lastX, lastY;
    Uint32 lastTime;
    int minDistance;  // minimum distance to record a new frame

    RecordingSession() {
        targetAnim = nullptr;
        isRecording = false;
        lastX = -1;
        lastY = -1;
        lastTime = 0;
        minDistance = 10;  // pixels
    }

    void startRecording(Animation* anim) {
        targetAnim = anim;
        isRecording = true;
        lastX = -1;
        lastY = -1;
        lastTime = SDL_GetTicks();
        cout << "Started recording animation: " << anim->name << endl;
    }

    void stopRecording() {
        isRecording = false;
        cout << "Stopped recording. Total frames: "
             << (targetAnim ? targetAnim->frameCount : 0) << endl;
    }

    void recordFrame(int currentX, int currentY, int costumeIndex) {
        if (!isRecording || !targetAnim) return;

        // Check if moved enough to record new frame
        if (lastX != -1) {
            int dx = currentX - lastX;
            int dy = currentY - lastY;
            int dist = (int)sqrt(dx*dx + dy*dy);

            if (dist < minDistance) {
                return;  // not moved enough
            }
        }

        // Add frame
        targetAnim->addFrame(currentX, currentY, costumeIndex);
        lastX = currentX;
        lastY = currentY;

        cout << "  Recorded frame " << targetAnim->frameCount
             << " at (" << currentX << "," << currentY << ")" << endl;
    }
};

// ============= Costume Manager =============
struct CostumeManager {
    Costume costumes[MAX_COSTUMES];
    int costumeCount;
    Animation animations[MAX_RECORDINGS];
    int animCount;
    RecordingSession recorder;
    SDL_Renderer* renderer;
    int activeCostume;

    CostumeManager(SDL_Renderer* r) {
        renderer = r;
        costumeCount = 0;
        animCount = 0;
        activeCostume = -1;
    }

    int addCostume(const char* name, const char* filePath) {
        if (costumeCount >= MAX_COSTUMES) return -1;

        costumes[costumeCount].setName(name);
        costumes[costumeCount].setPath(filePath);

        if (costumes[costumeCount].load(renderer)) {
            costumeCount++;
            return costumeCount - 1;
        }
        return -1;
    }

    void setActiveCostume(int index) {
        if (index >= 0 && index < costumeCount) {
            activeCostume = index;
        }
    }

    Costume* getActiveCostume() {
        if (activeCostume >= 0 && activeCostume < costumeCount) {
            return &costumes[activeCostume];
        }
        return nullptr;
    }

    int createAnimation(const char* name) {
        if (animCount >= MAX_RECORDINGS) return -1;

        animations[animCount].setName(name);
        animCount++;
        return animCount - 1;
    }

    Animation* getAnimation(int index) {
        if (index >= 0 && index < animCount) {
            return &animations[index];
        }
        return nullptr;
    }

    void startRecording(int animIndex) {
        if (animIndex >= 0 && animIndex < animCount) {
            recorder.startRecording(&animations[animIndex]);
        }
    }

    void stopRecording() {
        recorder.stopRecording();
    }

    void recordCurrentPosition(int x, int y) {
        recorder.recordFrame(x, y, activeCostume);
    }

    bool isRecording() const {
        return recorder.isRecording;
    }

    void renderActiveCostume(int x, int y) {
        if (activeCostume >= 0) {
            costumes[activeCostume].render(renderer, x, y);
        }
    }

    void listCostumes() const {
        cout << "\n=== Costumes (" << costumeCount << ") ===" << endl;
        for (int i = 0; i < costumeCount; i++) {
            cout << i << ": " << costumes[i].name
                 << " [" << costumes[i].filePath << "]"
                 << (i == activeCostume ? " (active)" : "")
                 << endl;
        }
        cout << "========================\n" << endl;
    }

    void listAnimations() const {
        cout << "\n=== Animations (" << animCount << ") ===" << endl;
        for (int i = 0; i < animCount; i++) {
            animations[i].print();
        }
        cout << "========================\n" << endl;
    }

    void cleanup() {
        for (int i = 0; i < costumeCount; i++) {
            costumes[i].unload();
        }
    }
};

#endif //UNTITLED75_COSTUME_H
