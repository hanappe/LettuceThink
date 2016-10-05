
// The code to 'daemonize' the process was copied from: 
// http://www-theorie.physik.unizh.ch/~dpotter/howto/daemonize
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>

#include "Stats.hpp"
#include "StatsUpdater.hpp"
#include "Options.hpp"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#ifdef _DEBUG
#define LOG_SIZE (128 * 1024)
#else
#define LOG_SIZE (512 * 1024)
#endif

void print_usage()
{
        fprintf(stderr, "Usage: famousd [options]\n");
        fprintf(stderr, "Try -h for help.\n");        
}

void print_help()
{
        printf("Options:\n");
        printf("--help\n");
        printf("--no-daemon: Don't daemonize, run the process in the foreground.\n");
        printf("--print: Print summary data to the standard output\n");
        printf("--daemon-name path\n");
        printf("--daemon-owner path\n");
        printf("--stay-root: Don't drop privileges, run the daemon as root.\n");
        printf("--lock-file path\n");
        printf("--log-file path\n");
        printf("--cpuid-file path\n");
        printf("--cpufreq-file path\n");
        printf("--lpbench-file path\n");
        printf("--pid-file path\n");
        printf("--mmap-file path\n");
}

int main(int argc, char *argv[]) 
{
        Options options;
        if (options.parse(argc, argv) != 0) {
                fprintf(stderr, "%s\n", options.message);
                if (options.usage) print_usage();
                exit(1);
        }
        if (options.help) {
                print_help();
                exit(1);
        }

        Stats* stats = new Stats();
        StatsUpdater statsUpdater(stats, &options);

        if (statsUpdater.init() != 0) {
                syslog(LOG_ERR, "stats.init failed");
                exit(EXIT_FAILURE);
        }

        /* Start monitoring */
        for (int step = 0; ; step++) {

                if (statsUpdater.update(step) != 0) {
                        syslog(LOG_ERR, "stats.update failed");
                        exit(EXIT_FAILURE);
                }

                stats->print();
        }
        
        return 0;
}
