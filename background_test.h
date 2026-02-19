//
// Created by MSI on 2/19/2026.
//

#ifndef UNTITLED75_BACKGROUND_TEST_H
#define UNTITLED75_BACKGROUND_TEST_H

#include "background.h"
#include <iostream>
using namespace std;
#ifdef USE_SDL_GFX
#include <SDL2/SDL2_gfx.h>
#endif
void testBackgroundManager() {
    cout << "\n========== BACKGROUND MANAGER TEST ==========\n" << endl;

    // Create dummy renderer (for testing)
    SDL_Renderer* dummyRenderer = nullptr;

    BackgroundManager manager(dummyRenderer, 800, 600);

    // ============= 7.1.2 Default Backgrounds =============
    cout << "--- Initializing Default Backgrounds ---" << endl;
    manager.initDefaultBackgrounds();
    manager.listDefaultBackgrounds();

    // Set a default background
    manager.setBackground(0);
    cout << "Active: " << manager.getActiveBackgroundName() << endl << endl;

    // ============= 7.1.1 Background Menu =============
    cout << "--- Testing Menu ---" << endl;
    manager.showBackgroundMenu();

    // ============= 7.1.3 Load from System =============
    cout << "--- Testing Load from System ---" << endl;
    manager.openFileDialog();
    manager.listDefaultBackgrounds();  // Will show both default and loaded

    // ============= 7.1.4 Internal Editor =============
    cout << "\n--- Testing Drawing Editor ---" << endl;
    manager.enterEditMode();

    // Simulate drawing events
    cout << "Simulating drawing..." << endl;
    manager.drawingTool.setTool(DrawingTool::TOOL_PEN);
    manager.drawingTool.setColor(255, 0, 0);  // Red
    manager.drawingTool.setSize(3);

    // Simulate mouse movement
    manager.drawingTool.startDraw(100, 100);
    manager.drawingTool.lastX = 100;
    manager.drawingTool.lastY = 100;

    // Simulate drawing line
    manager.drawingTool.stopDraw();

    // Test eraser
    manager.drawingTool.setTool(DrawingTool::TOOL_ERASER);
    manager.drawingTool.setSize(5);

    // Save drawing
    manager.saveCanvasAsBackground();
    manager.exitEditMode();

    // ============= Final State =============
    cout << "\n--- Final State ---" << endl;
    cout << "Total backgrounds: " << manager.bgCount << endl;
    cout << "Active: " << manager.getActiveBackgroundName() << endl;

    cout << "\n========== TEST END ==========\n" << endl;
}

#endif //UNTITLED75_BACKGROUND_TEST_H
