#ifndef UNTITLED7_PENDRAWING_H
#define UNTITLED7_PENDRAWING_H

#include <iostream>
#include <vector>
#include <string>
#include "PenSettings.h"
#include "PenSize.h"

struct Point {
    float x, y;
    Color color;
    float size;

    Point(float xPos, float yPos, const Color& c, float s)
            : x(xPos), y(yPos), color(c), size(s) {}
};

struct Sprite {
    std::string name;
    float x, y;
    Color penColor;
    float penSize;
    bool isPenDown;
    std::vector<Point> drawnPoints;
    std::vector<Point> stampedImages;

    Sprite(const std::string& n = "Sprite", float startX = 0, float startY = 0)
            : name(n), x(startX), y(startY), penColor(0, 0, 0),
              penSize(1.0f), isPenDown(false) {}
    void draw() {
        if (isPenDown) {
            drawnPoints.push_back(Point(x, y, penColor, penSize));
            std::cout << "✏️ " << name << " drawing at (" << x << "," << y
                      << ") with color RGB(" << penColor.r << ","
                      << penColor.g << "," << penColor.b
                      << ") and size " << penSize << std::endl;
        }
    }

    void moveTo(float newX, float newY) {
        x = newX;
        y = newY;
        draw();
    }

    void moveBy(float dx, float dy) {
        moveTo(x + dx, y + dy);
    }
    void eraseAll() {
        drawnPoints.clear();
        std::cout << "🧹 All drawings erased for " << name << std::endl;
    }

    void stamp() {
        stampedImages.push_back(Point(x, y, penColor, penSize));
        std::cout << "📸 Stamp: " << name << " at (" << x << "," << y
                  << ") with color RGB(" << penColor.r << ","
                  << penColor.g << "," << penColor.b
                  << ") and size " << penSize << std::endl;
    }
    void clearStamps() {
        stampedImages.clear();
        std::cout << "🧹 All stamps cleared for " << name << std::endl;
    }
    size_t getDrawingPointCount() const {
        return drawnPoints.size();
    }
    size_t getStampCount() const {
        return stampedImages.size();
    }
};

void updateSpritePen(Sprite& s, const Color& newColor, float newSize) {
    s.penColor = newColor;
    s.penSize = newSize;
    std::cout << "🖌️ Pen updated for " << s.name << std::endl;
}

void updateAllPens(std::vector<Sprite>& sprites, const Color& newColor, float newSize) {
    for (auto& s : sprites) {
        s.penColor = newColor;
        s.penSize = newSize;
    }
    std::cout << "🖌️ All pens updated" << std::endl;
}

void eraseAllSprites(std::vector<Sprite>& sprites) {
    for (auto& s : sprites) {
        s.eraseAll();
    }
}

void printSpriteInfo(const Sprite& s) {
    std::cout << "\n=== Sprite: " << s.name << " ===" << std::endl;
    std::cout << "Position: (" << s.x << ", " << s.y << ")" << std::endl;
    std::cout << "Pen Color: RGB(" << s.penColor.r << ","
              << s.penColor.g << "," << s.penColor.b << ")" << std::endl;
    std::cout << "Pen Size: " << s.penSize << std::endl;
    std::cout << "Pen State: " << (s.isPenDown ? "DOWN" : "UP") << std::endl;
    std::cout << "Drawn Points: " << s.getDrawingPointCount() << std::endl;
    std::cout << "Stamps: " << s.getStampCount() << std::endl;
}

#endif
