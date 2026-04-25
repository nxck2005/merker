#pragma once

namespace masses {
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
}