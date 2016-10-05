#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Options.hpp"

Options::Options() 
{
        help = 0;
        usage = 0;
        print = 1;
        Sn = 0;
}

int Options::parse(int argc, char** argv) 
{
        /* handle command options. on posix machines only */
        static struct option long_options[] = {
                {"help", 0, 0, 'h'},
                {"print", 0, 0, 'P'},
                {"sleep-durations", 1, 0, 'S'},
                {0, 0, 0, 0}
        };

        int option_index = 0;
        char* saveptr;
                
        while (1) {

                int c = getopt_long(argc, argv, "PhS:", 
                                    long_options, &option_index);
                if (c == -1) {
                        break;
                }
                        
                switch (c) {
                case 'h':
                        help = 1;
                        return 0;
                case 'P':
                        print = 1;
                        return 0;
                case 'S': {
                        while (1) {
                                char *t = strtok_r(optarg, ",", &saveptr);
                                if (t == 0) {
                                        break;
                                }
                                S[Sn] = atoi(t);
                                Sn++;
                                optarg = 0;
                        }}
                        break;
                case '?':
                        snprintf(message, 511, "Unknown option %c", optopt);
                        message[511] = 0;
                        usage = 1;
                        return -1;
                default:
                        snprintf(message, 511, "?? getopt returned character code 0%o ??", c);
                        message[511] = 0;
                        return -1;
                }
        }
        return 0;
}
