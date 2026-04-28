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
const int TELEMETRY_UPDATE_RATE = 1;

// Loop for sim worker thread
void simThreadFunc(State::simState& state) {
    while (state.running) {
        state.doSimTick();
    }
}

// GUI elements
const char* satellitePosGUI = TextFormat("");
const char* satelliteVelGUI = TextFormat("");
const char* satelliteVelAbsGUI = TextFormat("");
const char* satellitePosAbsGUI = TextFormat("");
const char* satelliteEpochsGUI = TextFormat("");
const char* satelliteCollidedGUI = TextFormat("");
const char* fpsText = TextFormat("");

int main() {
    State::simState state = {
        .parent = Bodies::earth,
        .satellite = Bodies::iss,
        .deltat = 1.0,
        .epoch = 0,
        .orbitDump = std::ofstream("orbit.csv"),
        .dumpFrequency = 60
    };

    // Launch the sim worker thread
    state.initSim();
    state.running = true;
    std::thread simThread(simThreadFunc, std::ref(state));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "merker visualiser");
    SetTargetFPS(240);

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
            if (currentEpoch % TELEMETRY_UPDATE_RATE == 0) {
                fpsText = TextFormat("FPS: %i", GetFPS());
                satellitePosGUI = TextFormat("Satellite POSX, POSY, POSZ: %f, %f, %f",
                    currentSat.posVector.x,
                    currentSat.posVector.y,
                    currentSat.posVector.z
                );
                satelliteVelGUI = TextFormat("Satellite OVELX, OVELY, OVELZ: %f, %f, %f",
                    currentSat.velVector.x,
                    currentSat.velVector.y,
                    currentSat.velVector.z
                );
                satelliteVelAbsGUI = TextFormat("Satellite OVEL: %f", glm::length(currentSat.velVector));
                satellitePosAbsGUI = TextFormat("Satellite POS: %f", glm::length(currentSat.posVector));

                // has the satellite went below the surface?
                if (glm::length(currentSat.posVector) <= state.parent.radius) {
                    satelliteCollidedGUI = TextFormat("Satellite COLLIDED: TRUE");
                } else {
                    satelliteCollidedGUI = TextFormat("Satellite COLLIDED: FALSE");
                }
                satelliteEpochsGUI = TextFormat("%i", currentEpoch);

                DrawText(fpsText, 10, 0, 20, GREEN);
                DrawText(satelliteEpochsGUI, 10, 20, 20, WHITE);
                DrawText(satellitePosGUI, 10, 40, 20, WHITE);
                DrawText(satelliteVelGUI, 10, 60, 20, WHITE);
                DrawText(satelliteVelAbsGUI, 10, 80, 20, WHITE);
                DrawText(satellitePosAbsGUI, 10, 100, 20, WHITE);
                DrawText(satelliteCollidedGUI, 10, 120, 20, WHITE);
            }
        EndDrawing();
    }

    state.running = false;
    if (simThread.joinable()) simThread.join();

    CloseWindow();

    return 0;
}
