/* All used bodies and vehicles defined here */

#include "bodies.h"
#include <cmath>

const double MOON_INCLINATION = 5.14;    // degrees
const double ISS_INCLINATION = 51.64;

namespace Bodies {
    const masses::Body earth = {
        .name             = "Earth",
        .mass             = 5.972e24,   // kg
        .radius           = 6.371e6,    // m
        .density          = 5515.0,     // kg/m^3
        .semiMajorAxis    = 1.496e11,   // m
        .orbitalPeriod    = 365.25,     // days
        .meanOrbitalSpeed = 29783.0,    // m/s
        .eccentricity     = 0.0167,
        .inclination      = 0.0,        // degrees (reference plane)
        .siderealDay      = 86164.0,    // s
        .axialTilt        = 23.44,      // degrees
        .surfaceGravity   = 9.807,      // m/s^2
        .escapeVelocity   = 11186.0,    // m/s
        .GM               = 3.986e14   // m^3/s^2
    };

    double i_rad = glm::radians(MOON_INCLINATION); // trying out an inclination
    masses::Vehicle moon = {
        .name = "Moon",
        .posVector = {3.844e8, 0.0, 0.0},
        .velVector = {0.0, 1018.4 * std::cos(i_rad), 1018.4 * std::sin(i_rad)}
    };

    // ISS approximate orbital parameters (LEO, ~408km altitude)
    // altitude = 408km → r = 6.371e6 + 408e3 = 6.779e6 m
    double r_iss   = 6.371e6 + 408e3;           // distance from Earth center
    double v_iss   = sqrt(earth.GM / r_iss);     // ~7667 m/s circular velocity
    masses::Vehicle iss = {
        .name = "International Space Station (ISS)",
        .posVector = {r_iss, 0.0, 0.0},
        .velVector = {0.0, v_iss * std::cos(glm::radians(ISS_INCLINATION)), v_iss * std::sin(glm::radians(ISS_INCLINATION))}
    };
}