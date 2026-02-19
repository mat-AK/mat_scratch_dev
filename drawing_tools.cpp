#include "drawing_tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

DrawingTools* CreateDrawingTools(int width, int height) {
    DrawingTools* tools = (DrawingTools*)malloc(sizeof(DrawingTools));
    if (!tools) return NULL;
    tools->canvasWidth = width;
    tools->canvasHeight = height;
    tools->canvas = SDL_CreateRGBSurface(0, width, height, 32,
                                         0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    if (!tools->canvas) {
        printf("❌ Failed to create canvas: %s\n", SDL_GetError());
        free(tools);
        return NULL;
    }
    SDL_FillRect(tools->canvas, NULL, SDL_MapRGB(tools->canvas->format, 255, 255, 255));

    tools->canvasTexture = NULL;
    tools->currentTool = TOOL_PEN;
    tools->currentColor = (DrawColor){0, 0, 0, 255};
    tools->penSize = 2;
    tools->line.drawing = false;
    tools->line.thickness = 2;
    tools->line.color = tools->currentColor;

    tools->circle.drawing = false;
    tools->circle.radius = 0;
    tools->circle.thickness = 2;
    tools->circle.filled = false;
    tools->circle.color = tools->currentColor;
    tools->rect.drawing = false;
    tools->rect.thickness = 2;
    tools->rect.filled = false;
    tools->rect.color = tools->currentColor;
    tools->textCount = 0;
    tools->undoIndex = -1;
    tools->undoCount = 0;
    for (int i = 0; i < 20; i++) {
        tools->undoBuffer[i] = NULL;
    }
    tools->mouseDown = false;
    tools->lastMouseX = 0;
    tools->lastMouseY = 0;
    tools->canvasModified = false;

    tools->colorWheelRect.x = 10;
    tools->colorWheelRect.y = 10;
    tools->colorWheelRect.w = 150;
    tools->colorWheelRect.h = 150;
    tools->colorWheelTexture = NULL;
    printf("✅ DrawingTools created (%dx%d)\n", width, height);
    return tools;
}

void DestroyDrawingTools(DrawingTools* tools) {
    if (!tools) return;

    if (tools->canvas) SDL_FreeSurface(tools->canvas);
    if (tools->canvasTexture) SDL_DestroyTexture(tools->canvasTexture);
    if (tools->colorWheelTexture) SDL_DestroyTexture(tools->colorWheelTexture);

    for (int i = 0; i < tools->textCount; i++) {
        if (tools->texts[i].texture) {
            SDL_DestroyTexture(tools->texts[i].texture);
        }
    }
    for (int i = 0; i < 20; i++) {
        if (tools->undoBuffer[i]) {
            SDL_FreeSurface(tools->undoBuffer[i]);
        }
    }
    free(tools);
    printf("✅ DrawingTools destroyed\n");
}

void SetCurrentTool(DrawingTools* tools, DrawingTool tool) {
    if (!tools) return;
    tools->currentTool = tool;

    const char* toolNames[] = {
            "PEN", "ERASER", "FILL", "TEXT", "LINE",
            "CIRCLE", "RECTANGLE", "SELECT", "FLIP_H", "FLIP_V", "CLEAR"
    };
    printf("✏️ Tool changed to: %s\n", toolNames[tool]);
}

DrawingTool GetCurrentTool(DrawingTools* tools) {
    return tools ? tools->currentTool : TOOL_PEN;
}

void SetCurrentColor(DrawingTools* tools, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!tools) return;

    tools->currentColor.r = r;
    tools->currentColor.g = g;
    tools->currentColor.b = b;
    tools->currentColor.a = a;
    tools->line.color = tools->currentColor;
    tools->circle.color = tools->currentColor;
    tools->rect.color = tools->currentColor;
}

DrawColor GetCurrentColor(DrawingTools* tools) {
    return tools->currentColor;
}

void SetPenSize(DrawingTools* tools, int size) {
    if (!tools) return;
    if (size < 1) size = 1;
    if (size > 50) size = 50;
    tools->penSize = size;
    tools->line.thickness = size;
    tools->circle.thickness = size;
    tools->rect.thickness = size;
}

static void DrawPointOnCanvas(DrawingTools* tools, int x, int y) {
    if (!tools || !tools->canvas) return;
    if (x < 0 || x >= tools->canvasWidth || y < 0 || y >= tools->canvasHeight) return;

    Uint32 color;
    if (tools->currentTool == TOOL_ERASER) {
        color = SDL_MapRGB(tools->canvas->format, 255, 255, 255);
    } else {
        color = SDL_MapRGBA(tools->canvas->format,
                            tools->currentColor.r,
                            tools->currentColor.g,
                            tools->currentColor.b,
                            tools->currentColor.a);
    }
    int size = tools->penSize;
    for (int dy = -size/2; dy <= size/2; dy++) {
        for (int dx = -size/2; dx <= size/2; dx++) {
            int px = x + dx;
            int py = y + dy;

            if (px >= 0 && px < tools->canvasWidth &&
                py >= 0 && py < tools->canvasHeight) {
                if (size > 1 && dx*dx + dy*dy > (size/2)*(size/2)) {
                    continue;
                }
                Uint32* pixels = (Uint32*)tools->canvas->pixels;
                pixels[py * tools->canvasWidth + px] = color;
            }
        }
    }
    tools->canvasModified = true;
}

static void DrawLineOnCanvas(DrawingTools* tools, int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        DrawPointOnCanvas(tools, x1, y1);

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

static void DrawCircleOnCanvas(DrawingTools* tools, int cx, int cy, int r, bool filled) {
    if (filled) {
        for (int y = -r; y <= r; y++) {
            for (int x = -r; x <= r; x++) {
                if (x*x + y*y <= r*r) {
                    DrawPointOnCanvas(tools, cx + x, cy + y);
                }
            }
        }
    } else {
        int x = 0;
        int y = r;
        int d = 1 - r;

        while (x <= y) {
            DrawPointOnCanvas(tools, cx + x, cy + y);
            DrawPointOnCanvas(tools, cx + y, cy + x);
            DrawPointOnCanvas(tools, cx - x, cy + y);
            DrawPointOnCanvas(tools, cx - y, cy + x);
            DrawPointOnCanvas(tools, cx + x, cy - y);
            DrawPointOnCanvas(tools, cx + y, cy - x);
            DrawPointOnCanvas(tools, cx - x, cy - y);
            DrawPointOnCanvas(tools, cx - y, cy - x);
            if (d < 0) {
                d += 2*x + 3;
            } else {
                d += 2*(x - y) + 5;
                y--;
            }
            x++;
        }
    }
}
static void DrawRectangleOnCanvas(DrawingTools* tools, int x1, int y1, int x2, int y2, bool filled) {
    int minX = (x1 < x2) ? x1 : x2;
    int maxX = (x1 < x2) ? x2 : x1;
    int minY = (y1 < y2) ? y1 : y2;
    int maxY = (y1 < y2) ? y2 : y1;

    if (filled) {
        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                DrawPointOnCanvas(tools, x, y);
            }
        }
    } else {
        for (int x = minX; x <= maxX; x++) {
            DrawPointOnCanvas(tools, x, minY);
            DrawPointOnCanvas(tools, x, maxY);
        }
        for (int y = minY; y <= maxY; y++) {
            DrawPointOnCanvas(tools, minX, y);
            DrawPointOnCanvas(tools, maxX, y);
        }
    }
}

void PushUndo(DrawingTools* tools) {
    if (!tools || !tools->canvas) return;
    if (tools->undoCount >= 20) {
        if (tools->undoBuffer[0]) {
            SDL_FreeSurface(tools->undoBuffer[0]);
        }
        for (int i = 1; i < 20; i++) {
            tools->undoBuffer[i-1] = tools->undoBuffer[i];
        }
        tools->undoCount--;
        tools->undoIndex--;
    }
    SDL_Surface* snapshot = SDL_CreateRGBSurface(0,
                                                 tools->canvasWidth,
                                                 tools->canvasHeight, 32,
                                                 0x00FF0000, 0x0000FF00,
                                                 0x000000FF, 0xFF000000);
    SDL_BlitSurface(tools->canvas, NULL, snapshot, NULL);
    tools->undoBuffer[tools->undoCount] = snapshot;
    tools->undoIndex = tools->undoCount;
    tools->undoCount++;
}

void Undo(DrawingTools* tools) {
    if (!tools || tools->undoIndex <= 0) return;
    tools->undoIndex--;

    if (tools->undoBuffer[tools->undoIndex]) {
        SDL_BlitSurface(tools->undoBuffer[tools->undoIndex], NULL, tools->canvas, NULL);
        tools->canvasModified = true;
        printf("↩️ Undo\n");
    }
}

void Redo(DrawingTools* tools) {
    if (!tools || tools->undoIndex >= tools->undoCount - 1) return;

    tools->undoIndex++;

    if (tools->undoBuffer[tools->undoIndex]) {
        SDL_BlitSurface(tools->undoBuffer[tools->undoIndex], NULL, tools->canvas, NULL);
        tools->canvasModified = true;
        printf("↪️ Redo\n");
    }
}

void ClearCanvas(DrawingTools* tools, Uint8 r, Uint8 g, Uint8 b) {
    if (!tools || !tools->canvas) return;
    PushUndo(tools);

    Uint32 color = SDL_MapRGB(tools->canvas->format, r, g, b);
    SDL_FillRect(tools->canvas, NULL, color);
    tools->canvasModified = true;
    printf("🧹 Canvas cleared\n");
}

void FlipCanvasHorizontal(DrawingTools* tools) {
    if (!tools || !tools->canvas) return;

    PushUndo(tools);

    Uint32* pixels = (Uint32*)tools->canvas->pixels;
    int width = tools->canvasWidth;
    int height = tools->canvasHeight;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width / 2; x++) {
            int oppositeX = width - 1 - x;
            Uint32 temp = pixels[y * width + x];
            pixels[y * width + x] = pixels[y * width + oppositeX];
            pixels[y * width + oppositeX] = temp;
        }
    }
    tools->canvasModified = true;
    printf("↔️ Canvas flipped horizontally\n");
}

void FlipCanvasVertical(DrawingTools* tools) {
    if (!tools || !tools->canvas) return;

    PushUndo(tools);
    Uint32* pixels = (Uint32*)tools->canvas->pixels;
    int width = tools->canvasWidth;
    int height = tools->canvasHeight;

    for (int y = 0; y < height / 2; y++) {
        for (int x = 0; x < width; x++) {
            int oppositeY = height - 1 - y;
            Uint32 temp = pixels[y * width + x];
            pixels[y * width + x] = pixels[oppositeY * width + x];
            pixels[oppositeY * width + x] = temp;
        }
    }
    tools->canvasModified = true;
    printf ("↕️ Canvas flipped vertically\n");
}

void FloodFill(DrawingTools* tools, int x, int y, DrawColor newColor) {
    if (!tools || !tools->canvas) return;
    if (x < 0 || x >= tools->canvasWidth || y < 0 || y >= tools->canvasHeight) return;

    PushUndo(tools);

    Uint32* pixels = (Uint32*)tools->canvas->pixels;
    int width = tools->canvasWidth;

    Uint32 targetColor = pixels[y * width + x];
    Uint32 newColorMapped = SDL_MapRGBA(tools->canvas->format,
                                        newColor.r, newColor.g,
                                        newColor.b, newColor.a);

    if (targetColor == newColorMapped) return;
    int* stackX = (int*)malloc(width * tools->canvasHeight * sizeof(int));
    int* stackY = (int*)malloc(width * tools->canvasHeight * sizeof(int));
    int stackTop = 0;

    stackX[stackTop] = x;
    stackY[stackTop] = y;
    stackTop++;

    while (stackTop > 0) {
        stackTop--;
        int cx = stackX[stackTop];
        int cy = stackY[stackTop];
        if (cx < 0 || cx >= width || cy < 0 || cy >= tools->canvasHeight) continue;

        int index = cy * width + cx;
        if (pixels[index] != targetColor) continue;
        pixels[index] = newColorMapped;

        stackX[stackTop] = cx + 1; stackY[stackTop] = cy; stackTop++;
        stackX[stackTop] = cx - 1; stackY[stackTop] = cy; stackTop++;
        stackX[stackTop] = cx; stackY[stackTop] = cy + 1; stackTop++;
        stackX[stackTop] = cx; stackY[stackTop] = cy - 1; stackTop++;
    }
    free(stackX);
    free(stackY);
    tools->canvasModified = true;
    printf("🪣 Flood fill completed\n");
}

void AddText(DrawingTools* tools, const char* text, int x, int y, TTF_Font* font) {
    if (!tools || tools->textCount >= 64) return;

    DrawText* newText = &tools->texts[tools->textCount];
    strcpy(newText->text, text);
    newText->x = x;
    newText->y = y;
    newText->color = tools->currentColor;
    newText->font = font;
    newText->texture = NULL;
    tools->textCount++;
    printf("📝 Text added: '%s' at (%d,%d)\n", text, x, y);
}

void RenderAllTexts(DrawingTools* tools, SDL_Renderer* renderer) {
    if (!tools || !renderer) return;

    for (int i = 0; i < tools->textCount; i++) {
        DrawText* txt = &tools->texts[i];

        if (!txt->texture && txt->font) {
            SDL_Surface* surf = TTF_RenderUTF8_Blended(txt->font, txt->text,
                                                       (SDL_Color){txt->color.r,
                                                                   txt->color.g,
                                                                   txt->color.b,
                                                                   txt->color.a});
            if (surf) {
                txt->texture = SDL_CreateTextureFromSurface(renderer, surf);
                txt->w = surf->w;
                txt->h = surf->h;
                SDL_FreeSurface(surf);
            }
        }
        if (txt->texture) {
            SDL_Rect dst = {txt->x, txt->y, txt->w, txt->h};
            SDL_RenderCopy(renderer, txt->texture, NULL, &dst);
        }
    }
}

void HandleMouseDown(DrawingTools* tools, int x, int y) {
    if (!tools) return;

    tools->mouseDown = true;
    tools->lastMouseX = x;
    tools->lastMouseY = y;

    switch (tools->currentTool) {
        case TOOL_PEN:
        case TOOL_ERASER:
            PushUndo(tools);
            DrawPointOnCanvas(tools, x, y);
            break;

        case TOOL_LINE:
            tools->line.drawing = true;
            tools->line.startX = x;
            tools->line.startY = y;
            tools->line.endX = x;
            tools->line.endY = y;
            break;

        case TOOL_CIRCLE:
            tools->circle.drawing = true;
            tools->circle.centerX = x;
            tools->circle.centerY = y;
            tools->circle.radius = 0;
            break;

        case TOOL_RECTANGLE:
            tools->rect.drawing = true;
            tools->rect.startX = x;
            tools->rect.startY = y;
            tools->rect.endX = x;
            tools->rect.endY = y;
            break;

        case TOOL_FILL:
            PushUndo(tools);
            FloodFill(tools, x, y, tools->currentColor);
            break;

        case TOOL_FLIP_H:
            PushUndo(tools);
            FlipCanvasHorizontal(tools);
            break;

        case TOOL_FLIP_V:
            PushUndo(tools);
            FlipCanvasVertical(tools);
            break;

        case TOOL_CLEAR:
            PushUndo(tools);
            ClearCanvas(tools, 255, 255, 255);
            break;
        default:
            break;
    }
}

void HandleMouseMove(DrawingTools* tools, int x, int y) {
    if (!tools || !tools->mouseDown) return;

    switch (tools->currentTool) {
        case TOOL_PEN:
        case TOOL_ERASER:
            DrawLineOnCanvas(tools, tools->lastMouseX, tools->lastMouseY, x, y);
            break;

        case TOOL_LINE:
            if (tools->line.drawing) {
                tools->line.endX = x;
                tools->line.endY = y;
            }
            break;

        case TOOL_CIRCLE:
            if (tools->circle.drawing) {
                int dx = x - tools->circle.centerX;
                int dy = y - tools->circle.centerY;
                tools->circle.radius = (int)sqrt(dx*dx + dy*dy);
            }
            break;

        case TOOL_RECTANGLE:
            if (tools->rect.drawing) {
                tools->rect.endX = x;
                tools->rect.endY = y;
            }
            break;
        default:
            break;
    }
    tools->lastMouseX = x;
    tools->lastMouseY = y;
}

void HandleMouseUp(DrawingTools* tools, int x, int y) {
    if (!tools) return;
    switch (tools->currentTool) {
        case TOOL_LINE:
            if (tools->line.drawing) {
                PushUndo(tools);
                DrawLineOnCanvas(tools,
                                 tools->line.startX, tools->line.startY,
                                 tools->line.endX, tools->line.endY);
                tools->line.drawing = false;
            }
            break;

        case TOOL_CIRCLE:
            if (tools->circle.drawing) {
                PushUndo(tools);
                DrawCircleOnCanvas(tools,
                                   tools->circle.centerX,
                                   tools->circle.centerY,
                                   tools->circle.radius,
                                   tools->circle.filled);
                tools->circle.drawing = false;
            }
            break;

        case TOOL_RECTANGLE:
            if (tools->rect.drawing) {
                PushUndo(tools);
                DrawRectangleOnCanvas(tools,
                                      tools->rect.startX, tools->rect.startY,
                                      tools->rect.endX, tools->rect.endY,
                                      tools->rect.filled);
                tools->rect.drawing = false;
            }
            break;

        default:
            break;
    }
    tools->mouseDown = false;
}

void RenderDrawingTools(DrawingTools* tools, SDL_Renderer* renderer) {
    if (!tools || !renderer) return;
    if (tools->canvasModified || !tools->canvasTexture) {
        if (tools->canvasTexture) {
            SDL_DestroyTexture(tools->canvasTexture);
        }

        tools->canvasTexture = SDL_CreateTextureFromSurface(renderer, tools->canvas);
        tools->canvasModified = false;
    }
    if (tools->canvasTexture) {
        SDL_RenderCopy(renderer, tools->canvasTexture, NULL, NULL);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (tools->line.drawing) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 128);
        SDL_RenderDrawLine(renderer,
                           tools->line.startX, tools->line.startY,
                           tools->line.endX, tools->line.endY);
    }
    if (tools->circle.drawing) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 128);
        for (int angle = 0; angle < 360; angle += 5) {
            float rad = angle * 3.14159 / 180;
            int cx = tools->circle.centerX + tools->circle.radius * cos(rad);
            int cy = tools->circle.centerY + tools->circle.radius * sin(rad);
            SDL_RenderDrawPoint(renderer, cx, cy);
        }
    }
    if (tools->rect.drawing) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 128);
        SDL_Rect preview = {
                (tools->rect.startX < tools->rect.endX) ? tools->rect.startX : tools->rect.endX,
                (tools->rect.startY < tools->rect.endY) ? tools->rect.startY : tools->rect.endY,
                abs(tools->rect.endX - tools->rect.startX),
                abs(tools->rect.endY - tools->rect.startY)
        };
        SDL_RenderDrawRect(renderer, &preview);
    }
}

void RenderToolUI(DrawingTools* tools, SDL_Renderer* renderer, TTF_Font* font) {
    if (!tools || !renderer) return;
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
    SDL_Rect toolbar = {0, 0, 200, tools->canvasHeight};
    SDL_RenderFillRect(renderer, &toolbar);

    int btnSize = 40;
    int btnX = 10;
    int btnY = 180;

    const char* toolSymbols[] = {
            "✏️", "🧽", "🪣", "📝", "📏", "⭕", "⬛", "👆", "↔️", "↕️", "🗑️"
    };

    for (int i = 0; i <= TOOL_CLEAR; i++) {
        SDL_Rect btnRect = {btnX, btnY + i * (btnSize + 5), btnSize, btnSize};
        if (i == tools->currentTool) {
            SDL_SetRenderDrawColor(renderer, 0, 120, 200, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        }
        SDL_RenderFillRect(renderer, &btnRect);
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &btnRect);

        if (font) {
            SDL_Surface* surf = TTF_RenderUTF8_Blended(font, toolSymbols[i],
                                                       (SDL_Color){255, 255, 255, 255});
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect dst = {btnRect.x + 5, btnRect.y + 5, btnSize - 10, btnSize - 10};
                SDL_RenderCopy(renderer, tex, NULL, &dst);
                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surf);
            }
        }
    }
    SDL_Rect colorRect = {btnX, btnY + 11 * (btnSize + 5), btnSize * 2, 40};
    SDL_SetRenderDrawColor(renderer,
                           tools->currentColor.r,
                           tools->currentColor.g,
                           tools->currentColor.b,
                           tools->currentColor.a);
    SDL_RenderFillRect(renderer, &colorRect);
    char sizeText[32];
    sprintf(sizeText, "Size: %d", tools->penSize);
    if (font) {
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, sizeText,
                                                   (SDL_Color){255, 255, 255, 255});
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dst = {btnX, colorRect.y + 50, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);