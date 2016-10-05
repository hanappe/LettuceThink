#ifndef __STATSUPDATER_HPP__
#define __STATSUPDATER_HPP__

class Stats;
class Options;

class StatsUpdater {
public:
        StatsUpdater(Stats* s, Options* options);
        virtual ~StatsUpdater();

        int init();
        int update(int step);

protected:

        Stats* _s;
        Options* _options;

        double getTime();
        int updateTime();
        int updateWatts();
        void openWattsUp();
};

#endif // __STATSUPDATER_HPP__
