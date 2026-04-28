/* Main loop */

#include <glm/ext/vector_double3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <fstream>
#include "bodies.h"
#include "state.h"
#include "raylib.h"
#include <thread>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MAX_FPS = 60;
const int TELEMETRY_UPDATE_RATE = 10000;

// Loop for sim worker thread
void simThreadFunc(State::simState& state) {
    while (state.running) {
        state.doSimTick();
    }
}

int main() {
    State::simState state = {
        .parent = Bodies::earth,
        .satellite = Bodies::iss,
        .deltat = 0.01,
        .epoch = 0,
        .orbitDump = std::ofstream("orbit.csv"),
        .dumpFrequency = 6000
    };

    // Launch the sim worker thread
    state.initSim();
    state.running = true;
    std::thread simThread(simThreadFunc, std::ref(state));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "merker visualiser");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // state.doSimTick(); // Now handled by thread

        // Capture state for rendering
        int currentEpoch;
        masses::Vehicle currentSat;
        {
            std::lock_guard<std::mutex> lock(state.mtx);
            currentEpoch = state.epoch;
            currentSat = state.satellite;
        }

        BeginDrawing();
            ClearBackground(BLACK);
                // 1. FPS
                DrawText(TextFormat("FPS: %i", GetFPS()), 10, 0, 20, GREEN);
                
                // 2. Epochs
                DrawText(TextFormat("Current Epoch: %i", currentEpoch), 10, 20, 20, WHITE);
                
                // 3. Pos Vector
                DrawText(TextFormat("Satellite POSX, POSY, POSZ: %f, %f, %f", 
                    currentSat.posVector.x, currentSat.posVector.y, currentSat.posVector.z), 
                    10, 40, 20, WHITE);
                    
                // 4. Vel Vector
                DrawText(TextFormat("Satellite OVELX, OVELY, OVELZ: %f, %f, %f", 
                    currentSat.velVector.x, currentSat.velVector.y, currentSat.velVector.z), 
                    10, 60, 20, WHITE);
                    
                // 5. Absolute Velocity
                DrawText(TextFormat("Satellite OVEL: %f", glm::length(currentSat.velVector)), 10, 80, 20, WHITE);
                
                // 6. Absolute Position
                DrawText(TextFormat("Satellite POS: %f", glm::length(currentSat.posVector)), 10, 100, 20, WHITE);

                // 7. Collision Check (No TextFormat needed for static strings)
                if (glm::length(currentSat.posVector) <= state.parent.radius) {
                    DrawText("Satellite COLLIDED: TRUE", 10, 120, 20, WHITE);
                } else {
                    DrawText("Satellite COLLIDED: FALSE", 10, 120, 20, WHITE);
                }

        EndDrawing();
    }

    state.running = false;
    if (simThread.joinable()) simThread.join();

    CloseWindow();

    return 0;
}
