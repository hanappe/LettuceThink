
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "Stats.hpp"

void Stats::print()
{
        if (wuReinit != 0) {
                printf("%32s: Reconnecting in %d seconds\n", 
                       "Power consumption", 60 - wuReinit);
        } else {
                printf("%lf\t%-.1f\t%-.1f\n", time - stime, watts, energy);
        }
}

