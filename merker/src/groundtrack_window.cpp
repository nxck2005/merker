/* Ground-track window: the child process launched by "Render Orbit".
 * Draws the satellite's ground track over the equirectangular Earth texture,
 * with a raygui slider for the number of overlaid orbits. */

#include "groundtrack.h"
#include "bodies.h"

#include "raylib.h"
#include "raygui.h"

#include <cmath>
#include <vector>

namespace groundtrack {

    // Map a (lon, lat) point in degrees to a pixel on a WxH equirectangular map.
    static Vector2 lonLatToPixel(const glm::dvec2& ll, int w, int h) {
        return (Vector2){
            (float)((ll.x + 180.0) / 360.0) * w,
            (float)((90.0 - ll.y) / 180.0) * h
        };
    }

    int runWindow() {
        const int W = 1000;
        const int H = 500;
        const int SAMPLES_PER_ORBIT = 512;
        const int MIN_ORBITS = 1;
        const int MAX_ORBITS = 16;

        InitWindow(W, H, "merker — ground track");
        SetTargetFPS(60);

        Texture2D earth = LoadTexture("../../../../src/resources/earth_albedo.png");
        SetTextureFilter(earth, TEXTURE_FILTER_BILINEAR);

        float orbitsF = 3.0f;
        int   builtFor = -1;   // orbit count the current trackData was computed for
        std::vector<std::vector<glm::dvec2>> trackData;

        while (!WindowShouldClose()) {
            const int orbits = (int)std::lround(orbitsF);

            // Recompute only when the slider actually changes the orbit count.
            if (orbits != builtFor) {
                trackData = tracks(Bodies::earth, Bodies::iss, orbits, SAMPLES_PER_ORBIT);
                builtFor = orbits;
            }

            BeginDrawing();
                ClearBackground(BLACK);

                // Background: Earth texture stretched to fill the window.
                DrawTexturePro(earth,
                    (Rectangle){ 0, 0, (float)earth.width, (float)earth.height },
                    (Rectangle){ 0, 0, (float)W, (float)H },
                    (Vector2){ 0, 0 }, 0.0f, WHITE);

                // Overlay each orbit oldest-first so the newest sits on top.
                for (const auto& orbit : trackData) {
                    for (size_t i = 0; i + 1 < orbit.size(); i++) {
                        // Skip the segment that jumps across the antimeridian, else it
                        // streaks straight across the map.
                        if (std::fabs(orbit[i + 1].x - orbit[i].x) > 180.0) continue;
                        DrawLineEx(lonLatToPixel(orbit[i], W, H),
                                   lonLatToPixel(orbit[i + 1], W, H), 2.0f, RED);
                    }
                }

                // UI: orbit-count slider + readout.
                GuiSliderBar((Rectangle){ 70, H - 28, W - 220, 18 }, "Orbits",
                             TextFormat("%d", orbits), &orbitsF,
                             (float)MIN_ORBITS, (float)MAX_ORBITS);
                DrawText(TextFormat("Ground track — %d orbit(s)", orbits), 10, 10, 20, RAYWHITE);

            EndDrawing();
        }

        UnloadTexture(earth);
        CloseWindow();
        return 0;
    }
}
