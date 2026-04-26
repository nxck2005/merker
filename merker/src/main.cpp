#include <climits>
#include <glm/ext/vector_double3.hpp>
#include <glm/glm.hpp>
#include <print>
#include <fstream>
#include "bodies.h"
#include "integrators.h"

using body = masses::Body;
using vehicle = masses::Vehicle;

const body earth = {
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

vehicle moon = {
    .posVector = {3.844e8, 0.0, 0.0},
    .velVector = {0.0, 1018.4, 0.0}
};

void printvec(const glm::dvec3& v) {
    std::println("({}, {}, {})", v.x, v.y, v.z);
}

int main() {
    glm::dvec3 x = glm::dvec3(1.0, 2.0, 3.0);
    std::println("earth and moon initialized");

    std::println("start:");
    printvec(moon.posVector);
    printvec(moon.velVector);

    std::ofstream file("orbit.csv");
    std::println(file, "xpos,ypos,zpos,xvel,yvel,zvel");

    const double dt = 1.0;
    int i = 0;
    while (i <= 2373649) {
        integrators::Verlet::doTick(earth, moon, dt);
        if (i % 3600 == 0) { // write every hour of data
            std::println("SIM: Running, at epoch {}", i);
            std::println(file, "{},{},{},{},{},{}", moon.posVector.x, moon.posVector.y, moon.posVector.z,
                                                    moon.velVector.x, moon.velVector.y, moon.velVector.z);
        }
        i++;
    }

}
