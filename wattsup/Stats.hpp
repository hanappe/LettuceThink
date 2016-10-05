#ifndef __STATS_HPP__
#define __STATS_HPP__

#include <stdio.h>


#define MAX_CPU 24
#define MAX_WU (2*MAX_CPU)

class Stats {
public:

        char hostID[64];

        // Flag indicating whether the values are in the process of
        // being updated.
        int updating;

        // The update number. This value is incremented at the end of
        // every call to update().
        int updateID;

        // Current time, in seconds
        double time;

        // The amount of time that the machine is running, in seconds
        double uptime;

        // Current power consumption, in Watt.
        double watts;

        // Amount of energy consumed since the start of the monitor,
        // in Joule.
        double energy;

        // The amount of errors that occured during the last call to
        // update().
        int errors;
        
        // Stuff
        int wuReinit;
        int wuConsecutiveErrors;
        double stime;        
        double lastTime;

        void print();
};

#endif // __STATS_HPP__
