/* Structure for bodies and vehicles. */
#pragma once

#include <glm/glm.hpp>
#include <print>
#include <string>

using vec3 = glm::dvec3;

namespace masses {

    /**
        A body is the "parent" object a Vehicle
        (satellite, craft etc) will revolve around.
    **/
    struct Body {

        std::string name;

        /**  PHYSICAL  **/
        const double mass;                // kg
        const double radius;              // metres
        const double density;             // kg/m^3

        /** AROUND THE SUN **/
        const double semiMajorAxis;       // m
        const double orbitalPeriod;       // days
        const double meanOrbitalSpeed;    // m/s
        const double eccentricity;        // no unit
        const double inclination;         // degrees

        /** ROTATION **/
        const double siderealDay;         // s
        const double axialTilt;           // degrees
        const double surfaceGravity;      // m/s^2
        const double escapeVelocity;      // m/s
        const double GM;                  // m^3/s^2

        void print() const {
            std::println("{} -> Mass: {:.3e} kg, GM: {:.3e}, Radius: {:.1f} m", name, mass, GM, radius);
        }

    };

    /**
        A vehicle is a object placed in orbit around,
        or on a parent Body.
    **/
    struct Vehicle {

        std::string name;

        /** ORBITAL PARAMS */
        vec3 posVector;
        vec3 velVector;

        void print() const {
            std::println("{} -> Pos: ({:.2f}, {:.2f}, {:.2f})", name, posVector.x, posVector.y, posVector.z);
            std::println("           Vel: ({:.2f}, {:.2f}, {:.2f})", velVector.x, velVector.y, velVector.z);
        }
    };
}
