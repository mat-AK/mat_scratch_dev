//
// Created by MSI on 2/19/2026.
//

#ifndef UNTITLED75_COSTUME_TEST_H
#define UNTITLED75_COSTUME_TEST_H
#include "costume.h"
#include <iostream>
using namespace std;

void testCostumeManager() {
    cout << "\n========== COSTUME MANAGER TEST ==========\n" << endl;

    SDL_Renderer* dummyRenderer = nullptr;

    CostumeManager manager(dummyRenderer);
    cout << "--- Adding Costumes ---" << endl;

    int catIdx = manager.addCostume("cat", "assets/cat.bmp");
    cout << "Added cat costume at index: " << catIdx << endl;

    int dogIdx = manager.addCostume("dog", "assets/dog.bmp");
    cout << "Added dog costume at index: " << dogIdx << endl;

    int birdIdx = manager.addCostume("bird", "assets/bird.bmp");
    cout << "Added bird costume at index: " << birdIdx << endl;

    manager.listCostumes();

    cout << "--- Setting Active Costume ---" << endl;
    manager.setActiveCostume(0);
    Costume* active = manager.getActiveCostume();
    if (active) {
        cout << "Active costume: " << active->name << endl;
    }

    cout << "\n--- Creating Animations ---" << endl;

    int walkAnim = manager.createAnimation("walk");
    cout << "Created walk animation at index: " << walkAnim << endl;

    int jumpAnim = manager.createAnimation("jump");
    cout << "Created jump animation at index: " << jumpAnim << endl;

    cout << "\n--- Recording Animation ---" << endl;

    manager.startRecording(walkAnim);

    manager.recordCurrentPosition(100, 100);
    manager.recordCurrentPosition(120, 100);
    manager.recordCurrentPosition(140, 100);
    manager.recordCurrentPosition(160, 100);
    manager.recordCurrentPosition(180, 100);

    manager.stopRecording();

    cout << "\n--- Adding Manual Frames ---" << endl;

    Animation* jump = manager.getAnimation(jumpAnim);
    if (jump) {
        jump->addFrame(100, 200, 1);
        jump->addFrame(100, 180, 1);
        jump->addFrame(100, 150, 1);
        jump->addFrame(100, 120, 1);
        jump->addFrame(100, 100, 1);
    }

    manager.listAnimations();

    cout << "\n--- Testing Animation Update ---" << endl;

    Animation* walk = manager.getAnimation(walkAnim);
    if (walk) {
        walk->start();

        cout << "Simulating 5 frames of animation:" << endl;
        for (int i = 0; i < 5; i++) {
            Frame* frame = walk->getCurrentFrame();
            if (frame) {
                cout << "  Frame " << walk->currentFrame << ": position ("
                     << frame->x << "," << frame->y << ")" << endl;
            }
            walk->update();
        }
    }

    cout << "\n========== TEST END ==========\n" << endl;
}


#endif //UNTITLED75_COSTUME_TEST_H
