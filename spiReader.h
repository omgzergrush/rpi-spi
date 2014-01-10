#ifndef _SPIREADER_H_
#define _SPIREADER_H_

#include <unistd.h>
#include <stdio.h>
#include "adis16485spi.h"

using namespace std;

class SpiReader
{
    public:
        struct InertiaData 
        {
            double xgyro;
            double ygyro;
            double zgyro;
            double xacc;
            double yacc;
            double zacc;
            InertiaData():xgyro(0.0),ygyro(0.0),zgyro(0.0),xacc(0.0),yacc(0.0),zacc(0.0) {}
        };

        SpiReader(void);
        ~SpiReader(void);
        int readSpi(void);
        const InertiaData& getAll(void) const;
        void printAll(void) const;

    private:
        unsigned char data[2];
        adis16485spi a2d;
        InertiaData inertia;
        int spiReadWriteErrorCode;
        
        unsigned int readData(adis16485spi& a2d, unsigned char* data, int length);
        double gyroOut(short int gyroRaw);
        double accOut(short int accRaw);
};

#endif
