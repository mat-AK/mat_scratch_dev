//
// Created by MSI on 2/19/2026.
//

#ifndef UNTITLED75_EXECUTION_PAUSE_COMPLETE_H
#define UNTITLED75_EXECUTION_PAUSE_COMPLETE_H

#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfx.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cstring>
using namespace std;
enum PauseState {
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STEPPING
};

// ============= Program Counter Structure =============
struct ProgramCounter {
    int currentLine;
    int nextLine;
    int lastExecutedLine;
    int totalLines;
    int* jumpAddresses;  // for pre-processed jumps

    ProgramCounter() {
        currentLine = 0;
        nextLine = 0;
        lastExecutedLine = -1;
        totalLines = 0;
        jumpAddresses = nullptr;
    }

    void init(int total) {
        totalLines = total;
        currentLine = 0;
        nextLine = 0;
        lastExecutedLine = -1;
        jumpAddresses = new int[total];
        for (int i = 0; i < total; i++) {
            jumpAddresses[i] = -1;
        }
    }

    void cleanup() {
        if (jumpAddresses) {
            delete[] jumpAddresses;
            jumpAddresses = nullptr;
        }
    }

    void goToLine(int line) {
        if (line >= 0 && line < totalLines) {
            currentLine = line;
            nextLine = line;
        }
    }

    void advance() {
        lastExecutedLine = currentLine;
        currentLine = nextLine;
        nextLine++;
    }

    void jumpTo(int targetLine) {
        nextLine = targetLine;
    }

    void print() const {
        cout << "PC: current=" << currentLine
             << ", next=" << nextLine
             << ", last=" << lastExecutedLine << endl;
    }
};

// ============= 8.2.1 Execution Tips Structure =============
struct ExecutionTips {
    int maxSteps;
    int currentSteps;
    Uint32 startTime;
    Uint32 pauseTime;
    Uint32 totalPausedTime;

    ExecutionTips() {
        maxSteps = 10000;  // prevent infinite loops
        currentSteps = 0;
        startTime = 0;
        pauseTime = 0;
        totalPausedTime = 0;
    }

    void start() {
        startTime = SDL_GetTicks();
        currentSteps = 0;
        totalPausedTime = 0;
    }

    void step() {
        currentSteps++;
    }

    bool shouldStop() const {
        return currentSteps >= maxSteps;
    }

    void onPause() {
        pauseTime = SDL_GetTicks();
    }

    void onResume() {
        if (pauseTime > 0) {
            totalPausedTime += SDL_GetTicks() - pauseTime;
            pauseTime = 0;
        }
    }

    Uint32 getRunningTime() const {
        Uint32 now = SDL_GetTicks();
        Uint32 paused = totalPausedTime;
        if (pauseTime > 0) {
            paused += now - pauseTime;
        }
        return (now - startTime) - paused;
    }

    void print() const {
        cout << "Steps: " << currentSteps << "/" << maxSteps
             << ", Time: " << getRunningTime() << "ms" << endl;
    }
};

// ============= 8.2.2 Pause Manager =============
struct PauseManager {
    PauseState state;
    ProgramCounter pc;
    ExecutionTips tips;

    // UI elements
    SDL_Rect pauseButtonRect;
    SDL_Rect resumeButtonRect;
    SDL_Rect stepButtonRect;
    SDL_Rect stopButtonRect;

    bool buttonHovered;
    int lastMouseX, lastMouseY;

    // Highlighting
    int highlightedLine;
    SDL_Color highlightColor;

    PauseManager() {
        state = STATE_RUNNING;
        buttonHovered = false;
        lastMouseX = lastMouseY = -1;
        highlightedLine = -1;
        highlightColor = {255, 255, 0, 128};  // yellow semi-transparent

        // Default button positions
        pauseButtonRect = {10, 10, 80, 40};
        resumeButtonRect = {100, 10, 80, 40};
        stepButtonRect = {190, 10, 80, 40};
        stopButtonRect = {280, 10, 80, 40};

    }


    // ============= Pause/Resume Controls =============
    void pause() {
        if (state == STATE_RUNNING) {
            state = STATE_PAUSED;
            tips.onPause();
            cout << "⏸️ Program PAUSED at line " << pc.currentLine << endl;
        }
    }

    void resume() {
        if (state == STATE_PAUSED || state == STATE_STEPPING) {
            state = STATE_RUNNING;
            tips.onResume();
            cout << "▶️ Program RESUMED from line " << pc.currentLine << endl;
        }
    }

    void stop() {
        state = STATE_RUNNING;  // reset to running but program will stop
        pc.goToLine(0);
        tips.start();
        cout << "⏹️ Program STOPPED" << endl;
    }

    void step() {
        if (state == STATE_PAUSED) {
            state = STATE_STEPPING;
            cout << "👣 Entered STEP MODE" << endl;
        }

        if (state == STATE_STEPPING) {
            cout << "   Executing step at line " << pc.currentLine << endl;
            // Will execute one block
            state = STATE_PAUSED;  // go back to paused after one step
        }
    }

    void togglePause() {
        if (state == STATE_RUNNING) {
            pause();
        } else if (state == STATE_PAUSED) {
            resume();
        }
    }

    // ============= Program Execution Control =============
    bool shouldExecuteNext() const {
        if (state == STATE_RUNNING) return true;
        if (state == STATE_STEPPING) return true;
        return false;
    }

    void beforeExecution() {
        tips.step();
        highlightedLine = pc.currentLine;
    }

    void afterExecution() {
        pc.advance();

        if (tips.shouldStop()) {
            cout << "⚠️ Maximum steps reached! Possible infinite loop." << endl;
            stop();
        }
    }

    // ============= 8.2.3 UI Requirements =============
    void handleMouseMotion(int x, int y) {
        lastMouseX = x;
        lastMouseY = y;

        buttonHovered = (x >= pauseButtonRect.x && x <= pauseButtonRect.x + pauseButtonRect.w &&
                         y >= pauseButtonRect.y && y <= pauseButtonRect.y + pauseButtonRect.h);
    }

    bool handleMouseClick(int x, int y) {
        // Check pause button
        if (x >= pauseButtonRect.x && x <= pauseButtonRect.x + pauseButtonRect.w &&
            y >= pauseButtonRect.y && y <= pauseButtonRect.y + pauseButtonRect.h) {
            togglePause();
            return true;
        }

        // Check resume button
        if (x >= resumeButtonRect.x && x <= resumeButtonRect.x + resumeButtonRect.w &&
            y >= resumeButtonRect.y && y <= resumeButtonRect.y + resumeButtonRect.h) {
            resume();
            return true;
        }

        // Check step button
        if (x >= stepButtonRect.x && x <= stepButtonRect.x + stepButtonRect.w &&
            y >= stepButtonRect.y && y <= stepButtonRect.y + stepButtonRect.h) {
            step();
            return true;
        }

        // Check stop button
        if (x >= stopButtonRect.x && x <= stopButtonRect.x + stopButtonRect.w &&
            y >= stopButtonRect.y && y <= stopButtonRect.y + stopButtonRect.h) {
            stop();
            return true;
        }

        return false;
    }

    void render(SDL_Renderer* renderer) {
        if (!renderer) return;

        // Draw buttons
        drawButton(renderer, pauseButtonRect, "Pause", state == STATE_RUNNING);
        drawButton(renderer, resumeButtonRect, "Resume", state == STATE_PAUSED);
        drawButton(renderer, stepButtonRect, "Step", state == STATE_PAUSED);
        drawButton(renderer, stopButtonRect, "Stop", true);

        // Draw status
        drawStatus(renderer);
    }

    void drawButton(SDL_Renderer* renderer, SDL_Rect rect, const char* text, bool enabled) {
        // Button background
        if (enabled) {
            SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);  // green
        } else {
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);  // gray
        }
        SDL_RenderFillRect(renderer, &rect);

        // Button border
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        // TODO: render text (requires TTF)
    }

    void drawStatus(SDL_Renderer* renderer) {
        char statusText[100];
        const char* stateStr = (state == STATE_RUNNING) ? "RUNNING" :
                               (state == STATE_PAUSED) ? "PAUSED" : "STEPPING";

        sprintf(statusText, "State: %s | Line: %d", stateStr, pc.currentLine);

        // Draw status background
        SDL_Rect statusRect = {10, 60, 300, 30};
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderFillRect(renderer, &statusRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &statusRect);
    }

    void highlightBlock(SDL_Renderer* renderer, SDL_Rect blockRect) {
        if (!renderer || highlightedLine < 0) return;

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, highlightColor.r, highlightColor.g,
                               highlightColor.b, highlightColor.a);
        SDL_RenderFillRect(renderer, &blockRect);
    }

    // ============= 8.2.4 Bonus Features =============
    void setButtonPosition(int x, int y, int w, int h) {
        pauseButtonRect = {x, y, w, h};
        resumeButtonRect = {x + w + 10, y, w, h};
        stepButtonRect = {x + 2*(w + 10), y, w, h};
        stopButtonRect = {x + 3*(w + 10), y, w, h};
    }

    void showExecutedBlocks() {
        cout << "Last executed: " << pc.lastExecutedLine
             << ", Current: " << pc.currentLine << endl;
    }

    void printDebugInfo() const {
        cout << "\n=== Pause Manager Debug ===" << endl;
        cout << "State: " << (state == STATE_RUNNING ? "RUNNING" :
                              state == STATE_PAUSED ? "PAUSED" : "STEPPING") << endl;
        pc.print();
        tips.print();
        cout << "==========================\n" << endl;
    }
    void cleanup() {
        if (pc.jumpAddresses != nullptr) {
            delete[] pc.jumpAddresses;
            pc.jumpAddresses = nullptr;
            cout << "Cleanup: jumpAddresses freed" << endl;
        }
    }
};

// ============= Test Function =============
void testPauseManager() {
    cout << "\n========== PAUSE MANAGER TEST ==========\n" << endl;

    PauseManager pm;
    pm.pc.init(10);  // 10 lines of program

    // Test 1: Basic pause/resume
    cout << "--- Test 1: Basic Pause/Resume ---" << endl;
    pm.pc.goToLine(3);
    pm.printDebugInfo();

    pm.pause();
    pm.printDebugInfo();

    pm.resume();
    pm.printDebugInfo();

    // Test 2: Step mode
    cout << "\n--- Test 2: Step Mode ---" << endl;
    pm.pause();
    pm.step();  // enter step mode

    for (int i = 0; i < 3; i++) {
        if (pm.shouldExecuteNext()) {
            pm.beforeExecution();
            cout << "Executing line " << pm.pc.currentLine << endl;
            pm.afterExecution();
        }
        pm.step();  // next step
    }

    // Test 3: Stop and reset
    cout << "\n--- Test 3: Stop ---" << endl;
    pm.stop();
    pm.printDebugInfo();

    // Test 4: Mouse interaction simulation
    cout << "\n--- Test 4: Mouse Interaction ---" << endl;
    pm.handleMouseMotion(15, 15);  // hover over pause button
    pm.handleMouseClick(15, 15);   // click pause

    pm.handleMouseClick(105, 15);  // click resume

    pm.handleMouseClick(195, 15);  // click step

    pm.handleMouseClick(285, 15);  // click stop

    // Test 5: Timing
    cout << "\n--- Test 5: Timing ---" << endl;
    pm.tips.start();
    SDL_Delay(100);
    pm.pause();
    SDL_Delay(200);
    pm.resume();
    SDL_Delay(50);

    cout << "Running time: " << pm.tips.getRunningTime() << "ms" << endl;

    pm.cleanup();
    cout << "\n========== TEST END ==========\n" << endl;
}



#endif //UNTITLED75_EXECUTION_PAUSE_COMPLETE_H
