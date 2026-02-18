#ifndef UNTITLED7_PENSIZE_H
#define UNTITLED7_PENSIZE_H

#include <iostream>

void setPenSize(float& currentSize, float newSize) {
    if (newSize > 0) {
        currentSize = newSize;
        std::cout << "✓ Pen size set to: " << currentSize << std::endl;
    } else {
        std::cout << "! Pen size must be positive. Keeping current size: "
                  << currentSize << std::endl;
    }
}

void changePenSize(float& currentSize, float amount) {
    float oldSize = currentSize;
    currentSize += amount;
    if (currentSize < 1) {
        currentSize = 1;
        std::cout << "! Minimum size reached (1)" << std::endl;
    }

    if (currentSize > 50) {
        currentSize = 50;
        std::cout << "! Maximum size reached (50)" << std::endl;
    }

    std::cout << "✓ Pen size changed from " << oldSize
              << " to " << currentSize << " (Δ" << amount << ")" << std::endl;
}
float getPenSize(const float& currentSize) {
    return currentSize;
}
bool isValidPenSize(float size) {
    return (size > 0 && size <= 50);
}

#endif
