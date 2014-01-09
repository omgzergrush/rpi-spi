/***********************************************************************
 * Based on
 * http://hertaville.com/2013/07/24/interfacing-an-spi-adc-mcp3008-chip-to-the-raspberry-pi-using-c/
 * Sample program that tests the adis16485spi class.
 * an adis16485spi class object (a2d) is created. the a2d object is instantiated
 * using the overloaded constructor. which opens the spidev0.0 device with
 * SPI_MODE_0 (MODE 0) (defined in linux/spi/spidev.h)
 *     
 * After conversion must merge data[1] and data[2] to get final result
 *
 * timespec diff() based on
 * http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
 *
 * *********************************************************************/
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include "adis16485spi.h"

using namespace std;

unsigned int readData(adis16485spi& a2d, unsigned char* data, int length)
{
        a2d.spiWriteRead(data, length );
        unsigned int pid = 0;
        //merge data[1] & data[2] to get result
        //(presumes int >= 32 bits)
        pid = (data[0]<< 8) & 0xffff; 
        pid |=  (data[1] & 0xff);
        return pid;
}

double gyroOut(short int gyroRaw)
{
    return (double)gyroRaw * 0.02;
}

double accOut(short int accRaw)
{
    return (double)accRaw * 0.25;
}

timespec diff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

int main(void)
{
    timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 500000000; // 500 ms
    timespec rem;
    rem.tv_nsec = 0;

    timespec time1, time2;

    // Open SPI communication, ADIS16485 uses 16 bit communication
    // but it fails (bug in spi/spidev.h?) so we use 8 bit instead.
    adis16485spi a2d("/dev/spidev0.0", SPI_MODE_3, 15000000, 8);
    
    unsigned char data[2]; //2x 8bits  

    cout << fixed;
    cout.precision(2);

    clock_gettime(CLOCK_REALTIME, &time1);
    time2 = time1;
    while(true)
    {
        data[0] = 0x12; // first byte transmitted (MSBs of 16 bits)
        data[1] = 0x00; // second byte transmitted (LSBs of 16 bits)
        unsigned int raw = readData(a2d, data, sizeof(data));
        double temp = 0.00565 * (double)raw + 25;

        data[0] = 0x16; 
        data[1] = 0x00;
        short int gyroRaw = 0;
        gyroRaw = readData(a2d, data, sizeof(data));
        double xgyro = gyroOut(gyroRaw);

        data[0] = 0x1A; 
        data[1] = 0x00;
        gyroRaw = readData(a2d, data, sizeof(data));
        double ygyro = gyroOut(gyroRaw);

        data[0] = 0x1E; 
        data[1] = 0x00;
        gyroRaw = readData(a2d, data, sizeof(data));
        double zgyro = gyroOut(gyroRaw);

        data[0] = 0x22; 
        data[1] = 0x00;
        short int accRaw = 0;
        accRaw = readData(a2d, data, sizeof(data));
        double xacc = accOut(accRaw);

        data[0] = 0x26; 
        data[1] = 0x00;
        accRaw = 0;
        accRaw = readData(a2d, data, sizeof(data));
        double yacc = accOut(accRaw);

        data[0] = 0x0E; 
        data[1] = 0x00;
        accRaw = 0;
        accRaw = readData(a2d, data, sizeof(data));
        double zacc = accOut(accRaw);
        
        cout << "------------------------------------------" << endl;  
        cout << "Temp:  " << temp << endl;
        cout << "Xgyro: " << xgyro << endl;
        cout << "Ygyro: " << ygyro << endl;
        cout << "Zgyro: " << zgyro << endl;
        cout << "Xacc:  " << xacc << endl;
        cout << "Yacc:  " << yacc << endl;
        cout << "Zacc:  " << zacc << endl;

        // calculate and print total cycle time difference
        timespec temptime = diff(time1, time2);
        cout << "cycle time: " << temptime.tv_sec << ":"<< temptime.tv_nsec;
        cout << " and rem: " << rem.tv_nsec << endl; 
        
        // calculate how much we need to sleep to have cycle time equal to delay
        clock_gettime(CLOCK_REALTIME, &temptime);
        timespec temptime2 = diff(time2, temptime);
        timespec sleeptime = diff(temptime2, delay);
        
        nanosleep(&sleeptime, &rem);
        //cout << sleeptime.tv_sec << ":" << sleeptime.tv_nsec << endl;


        // save old total cycle time and get new total cycle time
        // (must be done last)
        time1 = time2;
        clock_gettime(CLOCK_REALTIME, &time2);


    }
    return 0;
}
