
#ifndef __OPTIONS_HPP__
#define __OPTIONS_HPP__

class Options
{
public:

        Options();

        int parse(int argc, char** argv);

        int help;
        int usage;
        int print;
        char message[512];
        unsigned int S[20];
        int Sn;
};

#endif // __OPTIONS_HPP__


