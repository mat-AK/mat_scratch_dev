//
// Created by MSI on 2/19/2026.
//

#ifndef UNTITLED75_BACKGROUND_H
#define UNTITLED75_BACKGROUND_H
#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfx.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cstring>
#include <cstdio>
using namespace std;



// ============= Constants =============
#define MAX_BACKGROUNDS 50
#define MAX_NAME_LEN 50
#define MAX_PATH_LEN 256
#define DEFAULT_BG_COUNT 5

// ============= Background Structure =============
struct Background {
    char name[MAX_NAME_LEN];
    char filePath[MAX_PATH_LEN];
    SDL_Texture* texture;
    int width;
    int height;
    bool isLoaded;
    bool isDefault;

    Background() {
        name[0] = '\0';
        filePath[0] = '\0';
        texture = nullptr;
        width = 800;
        height = 600;
        isLoaded = false;
        isDefault = false;
    }

    void setName(const char* n) {
        strcpy(name, n);
    }

    void setPath(const char* path) {
        strcpy(filePath, path);
    }

    bool load(SDL_Renderer* renderer) {
        if (!renderer) return false;

        // Load image (simplified - in real code use IMG_LoadTexture)
        SDL_Surface* surface = SDL_LoadBMP(filePath);
        if (!surface) {
            cout << "Failed to load background: " << filePath << endl;
            return false;
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);
        width = surface->w;
        height = surface->h;
        SDL_FreeSurface(surface);

        if (texture) {
            isLoaded = true;
            cout << "Loaded background: " << name << endl;
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

    void render(SDL_Renderer* renderer, int screenW, int screenH) {
        if (!isLoaded || !texture) return;

        // Scale to fit screen
        SDL_Rect dest = {0, 0, screenW, screenH};
        SDL_RenderCopy(renderer, texture, nullptr, &dest);
    }

    void renderScaled(SDL_Renderer* renderer, int x, int y, int w, int h) {
        if (!isLoaded || !texture) return;

        SDL_Rect dest = {x, y, w, h};
        SDL_RenderCopy(renderer, texture, nullptr, &dest);
    }
};

// ============= Drawing Tool Structure =============
struct DrawingTool {
    enum ToolType {
        TOOL_PEN,
        TOOL_ERASER,
        TOOL_LINE,
        TOOL_RECT,
        TOOL_CIRCLE,
        TOOL_FILL
    };

    ToolType currentTool;
    SDL_Color penColor;
    int penSize;
    bool isDrawing;
    int lastX, lastY;

    DrawingTool() {
        currentTool = TOOL_PEN;
        penColor = {0, 0, 0, 255};
        penSize = 3;
        isDrawing = false;
        lastX = -1;
        lastY = -1;
    }

    void setTool(ToolType tool) {
        currentTool = tool;
        cout << "Tool changed to: " << getToolName() << endl;
    }

    void setColor(Uint8 r, Uint8 g, Uint8 b) {
        penColor.r = r;
        penColor.g = g;
        penColor.b = b;
    }

    void setSize(int size) {
        if (size > 0 && size < 50) {
            penSize = size;
        }
    }

    const char* getToolName() const {
        switch(currentTool) {
            case TOOL_PEN: return "Pen";
            case TOOL_ERASER: return "Eraser";
            case TOOL_LINE: return "Line";
            case TOOL_RECT: return "Rectangle";
            case TOOL_CIRCLE: return "Circle";
            case TOOL_FILL: return "Fill";
            default: return "Unknown";
        }
    }

    void startDraw(int x, int y) {
        isDrawing = true;
        lastX = x;
        lastY = y;
    }

    void stopDraw() {
        isDrawing = false;
        lastX = -1;
        lastY = -1;
    }

    void drawPoint(SDL_Renderer* renderer, int x, int y) {
        SDL_SetRenderDrawColor(renderer, penColor.r, penColor.g, penColor.b, penColor.a);

        if (currentTool == TOOL_ERASER) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // white for eraser
        }

        // Draw a circle of size penSize
        for (int dy = -penSize; dy <= penSize; dy++) {
            for (int dx = -penSize; dx <= penSize; dx++) {
                if (dx*dx + dy*dy <= penSize*penSize) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }
    }

    void drawLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2) {
        SDL_SetRenderDrawColor(renderer, penColor.r, penColor.g, penColor.b, penColor.a);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }

    void drawRect(SDL_Renderer* renderer, int x1, int y1, int x2, int y2) {
        SDL_SetRenderDrawColor(renderer, penColor.r, penColor.g, penColor.b, penColor.a);
        SDL_Rect rect;
        rect.x = (x1 < x2) ? x1 : x2;
        rect.y = (y1 < y2) ? y1 : y2;
        rect.w = abs(x2 - x1);
        rect.h = abs(y2 - y1);
        SDL_RenderDrawRect(renderer, &rect);
    }

    void update(SDL_Renderer* renderer, int x, int y) {
        if (!isDrawing || lastX < 0) return;

        switch(currentTool) {
            case TOOL_PEN:
            case TOOL_ERASER:
                drawLine(renderer, lastX, lastY, x, y);
                break;
            default:
                break;
        }

        lastX = x;
        lastY = y;
    }
};

// ============= Background Manager =============
struct BackgroundManager {
    Background backgrounds[MAX_BACKGROUNDS];
    int bgCount;
    int activeBackground;
    SDL_Renderer* renderer;
    int screenWidth;
    int screenHeight;

    // Drawing
    DrawingTool drawingTool;
    SDL_Texture* canvasTexture;
    bool inEditMode;

    BackgroundManager(SDL_Renderer* r, int w, int h) {
        renderer = r;
        screenWidth = w;
        screenHeight = h;
        bgCount = 0;
        activeBackground = -1;
        inEditMode = false;
        canvasTexture = nullptr;

        // Create blank canvas texture
        createBlankCanvas();
    }

    void createBlankCanvas() {
        if (!renderer) return;

        // Create a blank white texture for drawing
        SDL_Surface* surface = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32,
                                                    0, 0, 0, 0);
        if (surface) {
            SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 255, 255, 255));
            canvasTexture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
    }

    // ============= 7.1.1 Menu Access =============
    void showBackgroundMenu() {
        cout << "\n=== Background Menu ===" << endl;
        cout << "1. Default Backgrounds" << endl;
        cout << "2. Load from System" << endl;
        cout << "3. Draw Background" << endl;
        cout << "4. Current: " << getActiveBackgroundName() << endl;
        cout << "=====================\n" << endl;
    }

    // ============= 7.1.2 Default Backgrounds =============
    void initDefaultBackgrounds() {
        // Create default colored backgrounds
        addDefaultBackground("Blue Sky", 135, 206, 235);   // Sky blue
        addDefaultBackground("Green Grass", 34, 139, 34);  // Forest green
        addDefaultBackground("Night", 25, 25, 112);        // Midnight blue
        addDefaultBackground("Desert", 210, 180, 140);     // Sand
        addDefaultBackground("Gray", 128, 128, 128);       // Gray

        cout << "Added " << DEFAULT_BG_COUNT << " default backgrounds" << endl;
    }

    void addDefaultBackground(const char* name, Uint8 r, Uint8 g, Uint8 b) {
        if (bgCount >= MAX_BACKGROUNDS) return;

        // Create a colored surface
        SDL_Surface* surface = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32,
                                                    0, 0, 0, 0);
        if (surface) {
            SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, r, g, b));

            backgrounds[bgCount].setName(name);
            backgrounds[bgCount].texture = SDL_CreateTextureFromSurface(renderer, surface);
            backgrounds[bgCount].width = screenWidth;
            backgrounds[bgCount].height = screenHeight;
            backgrounds[bgCount].isLoaded = true;
            backgrounds[bgCount].isDefault = true;

            SDL_FreeSurface(surface);
            bgCount++;
        }
    }

    void listDefaultBackgrounds() {
        cout << "\n--- Default Backgrounds ---" << endl;
        for (int i = 0; i < bgCount; i++) {
            if (backgrounds[i].isDefault) {
                cout << i << ": " << backgrounds[i].name
                     << (i == activeBackground ? " (active)" : "") << endl;
            }
        }
        cout << "-------------------------\n" << endl;
    }

    bool setBackground(int index) {
        if (index >= 0 && index < bgCount && backgrounds[index].isLoaded) {
            activeBackground = index;
            cout << "Background set to: " << backgrounds[index].name << endl;
            return true;
        }
        return false;
    }

    // ============= 7.1.3 Load from System =============
    bool loadBackgroundFromFile(const char* filePath, const char* bgName) {
        if (bgCount >= MAX_BACKGROUNDS) return false;

        int index = bgCount;
        backgrounds[index].setName(bgName);
        backgrounds[index].setPath(filePath);

        if (backgrounds[index].load(renderer)) {
            bgCount++;
            return true;
        }

        return false;
    }

    // Simulate file dialog (simplified)
    void openFileDialog() {
        cout << "Opening file dialog..." << endl;
        cout << "In real implementation, this would open Windows file dialog" << endl;
        cout << "For testing, using default image: background.bmp" << endl;

        // Simulate loading a file
        loadBackgroundFromFile("background.bmp", "User Image");
    }

    // ============= 7.1.4 Internal Image Editor =============
    void enterEditMode() {
        inEditMode = true;
        cout << "Entered drawing mode. Use mouse to draw." << endl;
        cout << "Tools: P=Pen, E=Eraser, C=Clear, S=Save, ESC=Exit" << endl;
    }

    void exitEditMode() {
        inEditMode = false;
        cout << "Exited drawing mode." << endl;
    }

    void handleDrawingEvent(SDL_Event* event) {
        if (!inEditMode) return;

        switch(event->type) {
            case SDL_MOUSEBUTTONDOWN:
                if (event->button.button == SDL_BUTTON_LEFT) {
                    drawingTool.startDraw(event->button.x, event->button.y);
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event->button.button == SDL_BUTTON_LEFT) {
                    drawingTool.stopDraw();
                }
                break;

            case SDL_MOUSEMOTION:
                if (drawingTool.isDrawing) {
                    // Draw on canvas
                    SDL_SetRenderTarget(renderer, canvasTexture);
                    drawingTool.update(renderer, event->motion.x, event->motion.y);
                    SDL_SetRenderTarget(renderer, nullptr);
                }
                break;

            case SDL_KEYDOWN:
                handleDrawingKey(event->key.keysym.sym);
                break;
        }
    }

    void handleDrawingKey(SDL_Keycode key) {
        switch(key) {
            case SDLK_p:
                drawingTool.setTool(DrawingTool::TOOL_PEN);
                break;
            case SDLK_e:
                drawingTool.setTool(DrawingTool::TOOL_ERASER);
                break;
            case SDLK_c:
                clearCanvas();
                break;
            case SDLK_s:
                saveCanvasAsBackground();
                break;
            case SDLK_ESCAPE:
                exitEditMode();
                break;
            case SDLK_1:
                drawingTool.setSize(1);
                break;
            case SDLK_2:
                drawingTool.setSize(3);
                break;
            case SDLK_3:
                drawingTool.setSize(5);
                break;
        }
    }

    void clearCanvas() {
        SDL_SetRenderTarget(renderer, canvasTexture);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, nullptr);
        cout << "Canvas cleared" << endl;
    }

    void saveCanvasAsBackground() {
        if (bgCount >= MAX_BACKGROUNDS) return;

        // Create a new background from canvas
        backgrounds[bgCount].setName("Drawn Background");
        backgrounds[bgCount].texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                                         SDL_TEXTUREACCESS_TARGET,
                                                         screenWidth, screenHeight);

        // Copy canvas to new texture
        SDL_SetRenderTarget(renderer, backgrounds[bgCount].texture);
        SDL_RenderCopy(renderer, canvasTexture, nullptr, nullptr);
        SDL_SetRenderTarget(renderer, nullptr);

        backgrounds[bgCount].width = screenWidth;
        backgrounds[bgCount].height = screenHeight;
        backgrounds[bgCount].isLoaded = true;
        backgrounds[bgCount].isDefault = false;

        bgCount++;
        cout << "Saved drawing as background #" << (bgCount-1) << endl;
    }

    // ============= Rendering =============
    void render() {
        if (!renderer) return;

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);

        // Render active background
        if (activeBackground >= 0 && backgrounds[activeBackground].isLoaded) {
            backgrounds[activeBackground].render(renderer, screenWidth, screenHeight);
        }

        // If in edit mode, render canvas on top
        if (inEditMode && canvasTexture) {
            SDL_SetTextureBlendMode(canvasTexture, SDL_BLENDMODE_BLEND);
            SDL_RenderCopy(renderer, canvasTexture, nullptr, nullptr);
        }

        SDL_RenderPresent(renderer);
    }

    // ============= Utility =============
    const char* getActiveBackgroundName() const {
        if (activeBackground >= 0) {
            return backgrounds[activeBackground].name;
        }
        return "None";
    }

    void cleanup() {
        for (int i = 0; i < bgCount; i++) {
            backgrounds[i].unload();
        }
        if (canvasTexture) {
            SDL_DestroyTexture(canvasTexture);
            canvasTexture = nullptr;
        }
    }
};


#endif //UNTITLED75_BACKGROUND_H
