#pragma once

#include <glm/glm.hpp>
using vec3 = glm::dvec3;

namespace masses {

    /** 
        A body is the "parent" object a Vehicle
        (satellite, craft etc) will revolve around.
    **/
    struct Body {

        /**  PHYSICAL  **/
        double mass;                // kg
        double radius;              // metres
        double density;             // kg/m^3

        /** AROUND THE SUN **/
        double semiMajorAxis;       // m
        double orbitalPeriod;       // days
        double meanOrbitalSpeed;    // m/s
        double eccentricity;        // no unit
        double inclination;         // degrees

        /** ROTATION **/
        double siderealDay;         // s
        double axialTilt;           // degrees
        double surfaceGravity;      // m/s^2
        double escapeVelocity;      // m/s
        double GM;                  // m^3/s^2

    };

    /** 
        A vehicle is a object placed in orbit around,
        or on a parent Body.
    **/
    struct Vehicle {
        /** ORBITAL PARAMS */
        vec3 posVector;
        vec3 velVector;
        /** SIMULATION FUNCS **/
    };
}