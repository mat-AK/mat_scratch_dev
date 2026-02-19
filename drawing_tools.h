#ifndef UNTITLED7_DRAWING_TOOLS_H
#define UNTITLED7_DRAWING_TOOLS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef enum {
    TOOL_PEN,
    TOOL_ERASER,
    TOOL_FILL,
    TOOL_TEXT,
    TOOL_LINE,
    TOOL_CIRCLE,
    TOOL_RECTANGLE,
    TOOL_SELECT,
    TOOL_FLIP_H,
    TOOL_FLIP_V,
    TOOL_CLEAR,
} DrawingTool;

typedef struct {
    Uint8 r, g, b, a;
} DrawColor;

typedef struct {
    int x, y;
    DrawColor color;
    int size;
} DrawPoint;

typedef struct {
    char text[256];
    int x, y;
    DrawColor color;
    TTF_Font* font;
    SDL_Texture* texture;
    int w, h;
} DrawText;

typedef struct {
    int startX, startY;
    int endX, endY;
    bool drawing;
    DrawColor color;
    int thickness;
} LineTool;

typedef struct {
    int centerX, centerY;
    int radius;
    bool drawing;
    DrawColor color;
    int thickness;
    bool filled;
} CircleTool;

typedef struct {
    int startX, startY;
    int endX, endY;
    bool drawing;
    DrawColor color;
    int thickness;
    bool filled;
} RectangleTool;

typedef struct {
    DrawingTool currentTool;
    DrawColor currentColor;
    int penSize;
    SDL_Surface* canvas;
    SDL_Texture* canvasTexture;
    int canvasWidth;
    int canvasHeight;

    LineTool line;
    CircleTool circle;
    RectangleTool rect;
    DrawText texts[64];
    int textCount;
    SDL_Surface* undoBuffer[20];
    int undoIndex;
    int undoCount;
    bool mouseDown;
    int lastMouseX, lastMouseY;
    bool canvasModified;
    SDL_Rect colorWheelRect;
    SDL_Texture* colorWheelTexture;
    Uint32 fillTargetColor;

} DrawingTools;

DrawingTools* CreateDrawingTools(int width, int height);
void DestroyDrawingTools(DrawingTools* tools);
void SetCurrentTool(DrawingTools* tools, DrawingTool tool);
DrawingTool GetCurrentTool(DrawingTools* tools);

void SetCurrentColor(DrawingTools* tools, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void SetPenSize(DrawingTools* tools, int size);
DrawColor GetCurrentColor(DrawingTools* tools);
void HandleMouseDown(DrawingTools* tools, int x, int y);
void HandleMouseMove(DrawingTools* tools, int x, int y);
void HandleMouseUp(DrawingTools* tools, int x, int y);

void RenderDrawingTools(DrawingTools* tools, SDL_Renderer* renderer);
void RenderToolUI(DrawingTools* tools, SDL_Renderer* renderer, TTF_Font* font);
void ClearCanvas(DrawingTools* tools, Uint8 r, Uint8 g, Uint8 b);
void FlipCanvasHorizontal(DrawingTools* tools);
void FlipCanvasVertical(DrawingTools* tools);
void SaveCanvasToFile(DrawingTools* tools, const char* filename);
void LoadCanvasFromFile(DrawingTools* tools, const char* filename);

void PushUndo(DrawingTools* tools);
void Undo(DrawingTools* tools);
void Redo(DrawingTools* tools);
void FloodFill(DrawingTools* tools, int x, int y, DrawColor newColor);
void AddText(DrawingTools* tools, const char* text, int x, int y, TTF_Font* font);
void RenderAllTexts(DrawingTools* tools, SDL_Renderer* renderer);
SDL_Texture* CreateColorWheel(SDL_Renderer* renderer, int size);
DrawColor PickColorFromWheel(DrawingTools* tools, int mouseX, int mouseY);

#endif
