#include <iostream>
#include <SDL2/SDL.h>
#include "execution_pause_complete.h"

using namespace std;

int main(int argc, char* argv[]) {
    cout << "=== Fundamentals Project - Pause/Resume (Section 8.2) ===\n" << endl;

    // Run tests
    testPauseManager();

    // Simple example
    cout << "-- Simple Example ---" << endl;

    PauseManager pm;  // ✅ با P بزرگ
    pm.pc.init(5);

    // Simulate program execution
    pm.tips.start();

    for (int i = 0; i < 10; i++) {
        if (i == 3) pm.pause();
        if (i == 6) pm.resume();

        if (pm.shouldExecuteNext()) {
            pm.beforeExecution();
            cout << "Executing block " << pm.pc.currentLine << endl;
            pm.afterExecution();
        }

        SDL_Delay(100);
    }

    pm.printDebugInfo();
    pm.cleanup();

    cout << "\nProgram finished." << endl;

    return 0;
}