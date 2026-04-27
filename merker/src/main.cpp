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

int main() {
    std::println("SIM: init");
    
    std::println("Earth Data:");
    Bodies::earth.print();

    std::println("\nISS Initial State:");
    Bodies::iss.print();

    std::ofstream file("orbit.csv");
    std::println(file, "xpos,ypos,zpos,xvel,yvel,zvel");

    const double dt = 1.0;
    int i = 0;
    std::println("\nSIM: Starting integration...");
    while (i <= 5520 * 5) {     // around 5 iss orbits
        integrators::Verlet::doTick(Bodies::earth, Bodies::iss, dt);
        if (i % 60 == 0) { // write every minute of data
            std::println(file, "{},{},{},{},{},{}", Bodies::iss.posVector.x, Bodies::iss.posVector.y, Bodies::iss.posVector.z,
                                                    Bodies::iss.velVector.x, Bodies::iss.velVector.y, Bodies::iss.velVector.z);
        }
        i++;
    }
    std::println("SIM: Done. Data saved to orbit.csv");
}
