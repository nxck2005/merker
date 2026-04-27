/* Main loop */

#include <glm/ext/vector_double3.hpp>
#include <glm/glm.hpp>
#include <fstream>
#include "bodies.h"
#include "raylib.h"
#include "state.h"

int main() {
    State::simState state = {
        .parent = Bodies::earth,
        .satellite = Bodies::iss,
        .deltat = 1.0,
        .epoch = 0,
        .orbitDump = std::ofstream("orbit.csv"),
        .dumpFrequency = 60
    };

    state.initSim();
    state.runSim(100000);
}
