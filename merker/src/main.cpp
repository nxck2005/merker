/* Main loop */

#include <glm/ext/vector_double3.hpp>
#include <glm/glm.hpp>
#include <print>
#include <fstream>
#include "bodies.h"
#include "integrators.h"
#include "raylib.h"

using body = masses::Body;
using vehicle = masses::Vehicle;

void printvec(const glm::dvec3& v) {
    std::println("({}, {}, {})", v.x, v.y, v.z);
}

int main() {
    std::println("earth, moon, ISS initialized");
    std::println("ISS:");
    std::println("inclination: {} rad ({} deg)", glm::radians(ISS_INCLINATION), ISS_INCLINATION);
    std::println("cos: {}, sin: {}", std::cos(ISS_INCLINATION), std::sin(ISS_INCLINATION));

    std::println("start:");
    printvec(Bodies::iss.posVector);
    printvec(Bodies::iss.velVector);

    std::ofstream file("orbit.csv");
    std::println(file, "xpos,ypos,zpos,xvel,yvel,zvel");

    const double dt = 1.0;
    int i = 0;
    std::println("SIM: Starting sim...", i);
    while (i <= 5520 * 5) {     // around 5 iss orbits
        integrators::Verlet::doTick(Bodies::earth, Bodies::iss, dt);
        if (i % 60 == 0) { // write every minute of data
            std::println(file, "{},{},{},{},{},{}", Bodies::iss.posVector.x, Bodies::iss.posVector.y, Bodies::iss.posVector.z,
                                                    Bodies::iss.velVector.x, Bodies::iss.velVector.y, Bodies::iss.velVector.z);
        }
        i++;
    }

}
