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

    // Desaturate + dim the Earth texture so the red track reads as the foreground.
    // Mirrors raylib's default glsl330 texture shader, then mixes toward luminance.
    static const char* MUTE_FS =
        "#version 330\n"
        "in vec2 fragTexCoord;\n"
        "in vec4 fragColor;\n"
        "uniform sampler2D texture0;\n"
        "uniform vec4 colDiffuse;\n"
        "out vec4 finalColor;\n"
        "const float DESATURATE = 0.75;\n"
        "const float DIM        = 0.55;\n"
        "void main() {\n"
        "    vec4 texel = texture(texture0, fragTexCoord)*colDiffuse*fragColor;\n"
        "    float lum  = dot(texel.rgb, vec3(0.299, 0.587, 0.114));\n"
        "    finalColor = vec4(mix(texel.rgb, vec3(lum), DESATURATE)*DIM, texel.a);\n"
        "}\n";

    // Map a (lon, lat) point in degrees to a pixel inside the map rectangle.
    static Vector2 lonLatToPixel(const glm::dvec2& ll, const Rectangle& map) {
        return (Vector2){
            map.x + (float)((ll.x + 180.0) / 360.0) * map.width,
            map.y + (float)((90.0 - ll.y) / 180.0) * map.height
        };
    }

    int runWindow() {
        const int SAMPLES_PER_ORBIT = 512;
        const int MIN_ORBITS = 1;
        const int MAX_ORBITS = 16;

        // Layout: the map sits in a rect with margins for the axis labels, so
        // nothing (labels, slider) is drawn on top of the map itself.
        const int MARGIN_L = 48;   // latitude labels
        const int MARGIN_T = 36;   // title
        const int MARGIN_B = 64;   // longitude labels + slider
        const int MAP_W = 960, MAP_H = 480;          // 2:1, the equirectangular aspect
        const int W = MARGIN_L + MAP_W + 16;
        const int H = MARGIN_T + MAP_H + MARGIN_B;
        const Rectangle mapRect = { (float)MARGIN_L, (float)MARGIN_T, (float)MAP_W, (float)MAP_H };

        const int   GRID_STEP  = 30;   // degrees between gridlines
        const int   LABEL_SIZE = 10;
        const Color GRID_COL   = Fade(RAYWHITE, 0.18f);
        const Color AXIS_COL   = Fade(RAYWHITE, 0.45f);
        const Color LABEL_COL  = Fade(RAYWHITE, 0.75f);

        InitWindow(W, H, "merker — ground track");
        SetTargetFPS(60);

        Texture2D earth = LoadTexture("../../../../src/resources/earth_albedo.png");
        SetTextureFilter(earth, TEXTURE_FILTER_BILINEAR);
        Shader muteShader = LoadShaderFromMemory(nullptr, MUTE_FS);

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

                // Background: Earth texture, muted, filling the map rect.
                BeginShaderMode(muteShader);
                    DrawTexturePro(earth,
                        (Rectangle){ 0, 0, (float)earth.width, (float)earth.height },
                        mapRect, (Vector2){ 0, 0 }, 0.0f, WHITE);
                EndShaderMode();

                // Graticule. Everything goes through lonLatToPixel so the grid can't
                // drift out of sync with the track's projection.
                for (int lon = -180; lon <= 180; lon += GRID_STEP) {
                    const Vector2 top = lonLatToPixel({ (double)lon,  90.0 }, mapRect);
                    const Vector2 bot = lonLatToPixel({ (double)lon, -90.0 }, mapRect);
                    if (lon == 0) DrawLineEx(top, bot, 1.5f, AXIS_COL);   // prime meridian
                    else          DrawLineV(top, bot, GRID_COL);

                    // Longitude labels along the bottom edge, centred on the gridline.
                    // The ±180 ends would clip outside the window, so skip them.
                    if (lon == -180 || lon == 180) continue;
                    const char* txt = TextFormat("%d", lon);
                    DrawText(txt, (int)bot.x - MeasureText(txt, LABEL_SIZE) / 2,
                             (int)(mapRect.y + mapRect.height) + 6, LABEL_SIZE, LABEL_COL);
                }
                for (int lat = -90; lat <= 90; lat += GRID_STEP) {
                    const Vector2 left  = lonLatToPixel({ -180.0, (double)lat }, mapRect);
                    const Vector2 right = lonLatToPixel({  180.0, (double)lat }, mapRect);
                    if (lat == 0) DrawLineEx(left, right, 1.5f, AXIS_COL);   // equator
                    else          DrawLineV(left, right, GRID_COL);

                    // Latitude labels in the left gutter, right-aligned against the map.
                    const char* txt = TextFormat("%d", lat);
                    DrawText(txt, (int)mapRect.x - 6 - MeasureText(txt, LABEL_SIZE),
                             (int)left.y - LABEL_SIZE / 2, LABEL_SIZE, LABEL_COL);
                }
                DrawRectangleLinesEx(mapRect, 1.0f, Fade(RAYWHITE, 0.5f));

                // Axis titles. Latitude's sits at the right of the top margin rather
                // than rotated down the left gutter (raylib's default font can't do
                // that cheaply) and clear of the window title on the left.
                {
                    const char* lonTitle = "Longitude (deg)";
                    const char* latTitle = "Latitude (deg)";
                    DrawText(lonTitle,
                             (int)(mapRect.x + mapRect.width / 2) - MeasureText(lonTitle, LABEL_SIZE) / 2,
                             (int)(mapRect.y + mapRect.height) + 20, LABEL_SIZE, LABEL_COL);
                    DrawText(latTitle,
                             (int)(mapRect.x + mapRect.width) - MeasureText(latTitle, LABEL_SIZE),
                             (int)mapRect.y - 14, LABEL_SIZE, LABEL_COL);
                }

                // Overlay each orbit oldest-first so the newest sits on top.
                for (const auto& orbit : trackData) {
                    for (size_t i = 0; i + 1 < orbit.size(); i++) {
                        // Skip the segment that jumps across the antimeridian, else it
                        // streaks straight across the map.
                        if (std::fabs(orbit[i + 1].x - orbit[i].x) > 180.0) continue;
                        DrawLineEx(lonLatToPixel(orbit[i], mapRect),
                                   lonLatToPixel(orbit[i + 1], mapRect), 2.0f, RED);
                    }
                }

                // UI: orbit-count slider + readout.
                GuiSliderBar((Rectangle){ (float)MARGIN_L + 22, (float)H - 26, (float)MAP_W - 170, 18 },
                             "Orbits", TextFormat("%d", orbits), &orbitsF,
                             (float)MIN_ORBITS, (float)MAX_ORBITS);
                // Plain ASCII: raylib's default font has no glyph for an em dash.
                DrawText(TextFormat("Ground track - %d orbit(s)", orbits), 10, 9, 18, RAYWHITE);

            EndDrawing();
        }

        UnloadShader(muteShader);
        UnloadTexture(earth);
        CloseWindow();
        return 0;
    }
}
