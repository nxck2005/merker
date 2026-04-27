/* Integrators for math */
#pragma once
#include "masses.h"
#include <glm/geometric.hpp>

using vehicle = masses::Vehicle;
using body = masses::Body;
using vec3 = glm::dvec3;

namespace integrators {
    /**  
        Simpler but accumulates energy loss of 
         the order of delta-t^2.
    **/
    struct Euler {
        static void doTick(const body& parent, 
                          vehicle& satellite, 
                          double deltat) 
        {   
            // how far up is the satellite? (from center)
            double r = glm::length(satellite.posVector);

            // an arrow from the satellite to the centre of the body
            vec3 dir = glm::normalize(-satellite.posVector);

            // how much acceleration was added?
            vec3 accel = (dir) * (parent.GM / (r * r));


            satellite.velVector += accel * deltat;
            satellite.posVector += satellite.velVector * deltat;
        }
    };

    /** 
        Orbits stay closed with leapfrog, as 
        energy is conserved way better.
    **/
    struct Verlet {
        static void doTick(const body& parent,
                           vehicle& satellite,
                           double deltat)
        {
            // leapfrog
            vec3 acc      = glm::normalize(-satellite.posVector) * 
                            (parent.GM / glm::dot(satellite.posVector, satellite.posVector));
            satellite.posVector  += satellite.velVector * deltat + 0.5 * acc * deltat * deltat;
            vec3 accNew   = glm::normalize(-satellite.posVector) * (parent.GM / glm::dot(satellite.posVector, satellite.posVector));
            satellite.velVector  += 0.5 * (acc + accNew) * deltat;
        }
    };
}