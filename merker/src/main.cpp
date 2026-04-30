/* Main loop */

#include <chrono>
#include <glm/glm.hpp>
#include <fstream>
#include <thread>
#include <vector>
#include "masses.h"
#include "state.h"
#include "bodies.h"
#include "utils.h"

#include "raylib.h"
#include "rlgl.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int MAX_FPS = 60;
const int TELEMETRY_UPDATE_RATE = 10000;
const int MAX_TRAIL_LENGTH = 16000;
const bool USE_HDR = false; // need to fix hdr loading

// Loop for sim worker thread
void simThreadFunc(State::simState& state) {
    while (state.running) {
        state.doSimTick();
        std::this_thread::sleep_for(std::chrono::nanoseconds(5));
    }
}

int main() {
    State::simState state = {
        .parent = Bodies::earth,
        .satellite = Bodies::iss,
        .deltat = 0.01,
        .epoch = 0,
        .orbitDump = std::ofstream("orbit.csv"),
        .dumpFrequency = 6000,
        .dumpToCSV = false
    };

    // Launch the sim worker thread
    state.initSim();
    state.running = true;
    std::thread simThread(simThreadFunc, std::ref(state));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "merker visualiser");
    SetTargetFPS(0);

    // set up camera for 3d
    const double SCALE_FACTOR = 1000000.0;

    std::vector<Vector3> trail(MAX_TRAIL_LENGTH);
    int trailHead = 0;
    int trailCount = 0;

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 20.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // model and texturing
    float earthRadius = (float)(state.parent.radius / SCALE_FACTOR);

    Mesh sphereMesh = GenMeshSphere(earthRadius, 64, 64);
    Model earthModel = LoadModelFromMesh(sphereMesh);

    Texture2D earthTexture = LoadTexture("../../../../src/resources/earth_albedo.png");
    earthModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = earthTexture;

    // Skybox
    Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model skybox = LoadModelFromMesh(cube);

    // Load skybox shader and set required locations
    // NOTE: Some locations are automatically set at shader loading
    // shaders from raylib examples, will slim this down later
    skybox.materials[0].shader = LoadShader(TextFormat("../../../../src/resources/shaders/glsl%i/skybox.vs", GLSL_VERSION),
                                            TextFormat("../../../../src/resources/shaders/glsl%i/skybox.fs", GLSL_VERSION));

    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "environmentMap"), (int[1]){ MATERIAL_MAP_CUBEMAP }, SHADER_UNIFORM_INT);
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "doGamma"), (int[1]){ USE_HDR? 1 : 0 }, SHADER_UNIFORM_INT);
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "vflipped"), (int[1]){ USE_HDR? 1 : 0 }, SHADER_UNIFORM_INT);

    // Load cubemap shader and setup required shader locations
    Shader shdrCubemap = LoadShader(TextFormat("../../../../src/resources/shaders/glsl%i/cubemap.vs", GLSL_VERSION),
                                    TextFormat("../../../../src/resources/shaders/glsl%i/cubemap.fs", GLSL_VERSION));

    SetShaderValue(shdrCubemap, GetShaderLocation(shdrCubemap, "equirectangularMap"), (int[1]){ 0 }, SHADER_UNIFORM_INT);

    char skyboxFileName[256] = { 0 };

    TextCopy(skyboxFileName, "../../../../src/resources/space.png");

    // Load HDR panorama (sphere) texture
    Texture2D panorama = LoadTexture(skyboxFileName);

    // Generate cubemap (texture with 6 quads-cube-mapping) from panorama HDR texture
    // NOTE 1: New texture is generated rendering to texture, shader calculates the sphere->cube coordinates mapping
    // NOTE 2: It seems on some Android devices WebGL, fbo does not properly support a FLOAT-based attachment,
    // despite texture can be successfully created.. so using PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 instead of PIXELFORMAT_UNCOMPRESSED_R32G32B32A32
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = utils::GenTextureCubemap(shdrCubemap, panorama, 1024, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    UnloadTexture(panorama);        // Texture not required anymore, cubemap already generated

    while (!WindowShouldClose()) {

        // Snapshot state for rendering
        int currentEpoch;
        masses::Vehicle currentSat;
        {
            std::lock_guard<std::mutex> lock(state.mtx);
            currentEpoch = state.epoch;
            currentSat = state.satellite;
        }
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        BeginDrawing();
            ClearBackground(BLACK);

            // 3D pass
            BeginMode3D(camera);

                // We are inside the cube, we need to disable backface culling!
                rlDisableBackfaceCulling();
                rlDisableDepthMask();

                DrawModel(skybox, (Vector3){0, 0, 0}, 1.0f, WHITE);

                rlEnableBackfaceCulling();
                rlEnableDepthMask();

                DrawModel(earthModel, (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);

                // draw satellite
                Vector3 satPos = {
                    (float)(currentSat.posVector.x / SCALE_FACTOR),
                    (float)(currentSat.posVector.y / SCALE_FACTOR),
                    (float)(currentSat.posVector.z / SCALE_FACTOR)
                };

                trail[trailHead] = satPos;
                trailHead = (trailHead + 1) % MAX_TRAIL_LENGTH;
                if (trailCount < MAX_TRAIL_LENGTH) trailCount++;

                DrawSphere(satPos, 0.2f, RED);

                // Draw the ring buffer trail
                if (trailCount > 1) {
                    // Find the oldest point in the buffer
                    int oldestIdx = (trailHead + MAX_TRAIL_LENGTH - trailCount) % MAX_TRAIL_LENGTH;

                    for (int i = 0; i < trailCount - 1; i++) {
                        int current = (oldestIdx + i) % MAX_TRAIL_LENGTH;
                        int next = (oldestIdx + i + 1) % MAX_TRAIL_LENGTH;

                        // Fade the trail based on how old the point is!
                        // i = 0 is oldest (most transparent), i = trailCount is newest (most opaque)
                        float alpha = (float)i / trailCount;

                        DrawLine3D(trail[current], trail[next], Fade(RED, alpha));
                    }
                }

            EndMode3D();

            // 2D text

                // 1. FPS
                // DrawText(TextFormat("FPS: %i", GetFPS()), 10, 0, 20, GREEN);
                DrawFPS(10, 0);

                // 2. Epochs
                DrawText(TextFormat("Current EPOCH: %i", currentEpoch), 10, 20, 20, WHITE);

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

    UnloadTexture(earthTexture);
    UnloadModel(earthModel);
    UnloadShader(skybox.materials[0].shader);
    UnloadTexture(skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture);
    UnloadModel(skybox);        // Unload skybox model

    CloseWindow();

    return 0;
}

// Generate cubemap texture from HDR texture
