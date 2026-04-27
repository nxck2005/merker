/* Holds the state of the simulation */

#pragma once
#include "masses.h"
#include "integrators.h"
#include <fstream>

namespace State {
    struct simState {
        const masses::Body parent;          // parent body
        masses::Vehicle satellite;          // satellite
        double deltat = 1;                  // speed of the sim
        int epoch = 0;                      // number of iterations done
        std::ofstream orbitDump;            // the orbit csv dump
        int dumpFrequency = 60;             // how often to dump data, default = 1 minute (60s)

        void initSim() {
            std::println("SIM: init");
            
            std::println("Parent State:");
            parent.print();

            std::println("\nSatellite State:");
            satellite.print();

            std::println(orbitDump, "xpos,ypos,zpos,xvel,yvel,zvel");
        }
        
        // dump to orbit.csv
        void doSimTick() {
            integrators::Verlet::doTick(parent, satellite, deltat);
            if (epoch % dumpFrequency == 0) {
                std::println(orbitDump, "{},{},{},{},{},{}", satellite.posVector.x, satellite.posVector.y, satellite.posVector.z,
                                                    satellite.velVector.x, satellite.velVector.y, satellite.velVector.z);
            }
            epoch++;
        }

        void runSim(int epochs) {
            std::println("\nSIM: Starting integration for {} epochs...", epochs);
            for (int i = 1; i <= epochs; i++) doSimTick();
            std::println("SIM: Done. Data saved to orbit.csv");
        }
    };
}