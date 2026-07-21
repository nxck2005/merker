/* Ground-track computation and window (separate-process view).
 *
 * Pure math here (no raylib): given a parent body and a satellite's initial
 * state, integrate N orbits and project each sample to a sub-satellite
 * (longitude, latitude) point on the rotating Earth. runWindow() (defined in
 * groundtrack_window.cpp) draws these over the equirectangular Earth texture.
 */
#pragma once

#include "masses.h"
#include "integrators.h"
#include <glm/geometric.hpp>
#include <cmath>
#include <vector>

namespace groundtrack {

    // Wrap a longitude in degrees to [-180, 180).
    inline double wrapTo180(double lon) {
        lon = std::fmod(lon + 180.0, 360.0);
        if (lon < 0.0) lon += 360.0;
        return lon - 180.0;
    }

    // Orbital period (seconds) via vis-viva: a = 1 / (2/r - v^2/mu), T = 2*pi*sqrt(a^3/mu).
    inline double period(const masses::Body& parent, const masses::Vehicle& sat) {
        const double r  = glm::length(sat.posVector);
        const double v  = glm::length(sat.velVector);
        const double mu = parent.GM;
        const double a  = 1.0 / (2.0 / r - (v * v) / mu);
        return 2.0 * M_PI * std::sqrt((a * a * a) / mu);
    }

    // Ground track for `numOrbits` consecutive orbits. Returns one (lon,lat) list
    // per orbit (degrees) so callers can overlay them. Longitude includes Earth's
    // rotation (360 * t / siderealDay), so successive orbits drift westward.
    inline std::vector<std::vector<glm::dvec2>>
    tracks(const masses::Body& parent, masses::Vehicle sat, int numOrbits, int samplesPerOrbit) {
        std::vector<std::vector<glm::dvec2>> out;
        if (numOrbits < 1 || samplesPerOrbit < 2) return out;

        const double T  = period(parent, sat);
        const double dt = T / samplesPerOrbit;
        double t = 0.0;

        out.reserve(numOrbits);
        for (int o = 0; o < numOrbits; o++) {
            std::vector<glm::dvec2> orbit;
            orbit.reserve(samplesPerOrbit);
            for (int s = 0; s < samplesPerOrbit; s++) {
                const double r    = glm::length(sat.posVector);
                const double lat  = glm::degrees(std::asin(sat.posVector.z / r));
                const double lonI = glm::degrees(std::atan2(sat.posVector.y, sat.posVector.x));
                const double lon  = wrapTo180(lonI - 360.0 * (t / parent.siderealDay));
                orbit.emplace_back(lon, lat);

                integrators::Verlet::doTick(parent, sat, dt);
                t += dt;
            }
            out.push_back(std::move(orbit));
        }
        return out;
    }

    // Runs the ground-track window (blocking). Defined in groundtrack_window.cpp.
    int runWindow();
}
