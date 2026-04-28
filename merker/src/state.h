/* Holds the state of the simulation */

#pragma once
#include "masses.h"
#include "integrators.h"
#include <fstream>
#include <mutex>
#include <atomic>

namespace State {
    struct simState {

        /** Bodies involved **/
        const masses::Body parent;              // parent body
        masses::Vehicle satellite;              // satellite

        /** Simulation params **/
        double deltat = 1;                      // how much time passes per epoch?
        int epoch = 0;                          // number of iterations dones

        /** Simulation threads **/
        std::atomic<bool> running{false};     // keep true while the worker thread runs, allows to do ticks
        mutable std::mutex mtx;                 // protects satellite and epoch

        /** Output params **/
        std::ofstream orbitDump;                // the orbit csv dump
        int dumpFrequency = 60;                 // how often to dump data, default = 60 epochs

        /** Simulation functions **/

        // Initialise the simulation, write the CSV headers etc.
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

            // We calculate physics outside the lock if possible,
            // but for simplicity here we lock the whole update.
            {
                std::lock_guard<std::mutex> lock(mtx);
                integrators::Verlet::doTick(parent, satellite, deltat);
                epoch++;
            }

            // snapshot storage
            int currentEpoch;
            masses::Vehicle currentSat;

            {
                std::lock_guard<std::mutex> lock(mtx);
                // a snapshot is dumped, from here
                currentEpoch = epoch;
                currentSat = satellite;
            }

            // Disk I/O is slow, so we do it outside the lock
            if (currentEpoch % dumpFrequency == 0) {
                std::println(orbitDump, "{},{},{},{},{},{}", currentSat.posVector.x, currentSat.posVector.y, currentSat.posVector.z,
                                                            currentSat.velVector.x, currentSat.velVector.y, currentSat.velVector.z);
                std::println("SIM: reached and dumped epoch {} data to dumpfile", currentEpoch);
            }

        }

        void runSim(int epochs) {

            std::println("\nSIM: Starting integration for {} epochs...", epochs);

            for (int i = 1; i <= epochs; i++) doSimTick();

            std::println("SIM: Done. Data saved to orbit.csv");

        }
    };
}
