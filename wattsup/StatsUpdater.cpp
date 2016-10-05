
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <syslog.h>
#include <math.h>
#include <string>
#include <new>

#include "StatsUpdater.hpp"
#include "Stats.hpp"
#include "wu_private.h"
#include "Options.hpp"

StatsUpdater::StatsUpdater(Stats* s, Options* options)
{
        _s = s;
        _options = options;
}

StatsUpdater::~StatsUpdater()
{
        wu_close();
}

void StatsUpdater::openWattsUp() 
{
        _s->wuReinit = 0;
        _s->wuConsecutiveErrors = 0;
        if (wu_open() != 0) {
                _s->errors++;
                _s->wuReinit = 1;
        }
}

int StatsUpdater::init() 
{
        openWattsUp();
        if (_s->wuReinit > 0) {
                fprintf(stderr, "failed to open the Watt's Up device");
        }

        _s->errors = 0;
        _s->updating = 0;
        _s->updateID = 0;
        _s->energy = 0.0;

        _s->stime = getTime();
        if (_s->stime < 0) return -1;

        _s->lastTime = _s->stime;

        return 0;
}
        
double StatsUpdater::getTime() 
{
        struct timeval tv;
        if (gettimeofday(&tv, NULL) != 0) {
                syslog(LOG_ERR, "gettimeofday failed, code=%d (%s)", 
                       errno, strerror(errno));
                return -1.0;
        }
        return (double) tv.tv_sec + (double) tv.tv_usec / 1000000.0;
}

int StatsUpdater::updateTime() 
{
        _s->time = getTime();
        if (_s->time < 0.0)
                return -1;
        return 0;
}

int StatsUpdater::updateWatts() 
{
        int r = -1;
        if (_s->wuReinit == 0) {
                _s->watts = wu_watts();
                if (_s->watts < 0.0) {
                        _s->wuConsecutiveErrors++;
                        if (_s->wuConsecutiveErrors == 50) {
                                wu_close();
                                _s->wuReinit = 1;
                        }
                } else {
                        _s->wuConsecutiveErrors = 0;
                        double t = getTime();
                        _s->energy += _s->watts * (t - _s->lastTime);
                        _s->lastTime = t;
                        r = 0;
                }
        } else {
                _s->wuReinit++;
                if (_s->wuReinit == 60) {
                        openWattsUp();
                }
                if (_s->wuReinit == 0) 
                        r = updateWatts();
                else 
                        sleep(1);                
        }
        return r;
}

int StatsUpdater::update(int step) 
{
        _s->errors = 0;
        _s->updating = 1;

        if (updateTime() != 0)
                _s->errors++;

        if (updateWatts() != 0)
                _s->errors++;

        _s->updating = 0;
        _s->updateID++;
        return 0;
}

