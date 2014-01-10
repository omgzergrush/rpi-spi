#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include "spiReader.h"

using namespace std;

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
    SpiReader spi;
    SpiReader::InertiaData idata;

    timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 500000000; // 500 ms
    timespec rem;
    rem.tv_nsec = 0;

    timespec time1, time2;

    clock_gettime(CLOCK_REALTIME, &time1);
    time2 = time1;
    while(true)
    {
        spi.readSpi();
        idata = spi.getAll();
        //cout << idata.xgyro << endl;
        spi.printAll();

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
        // (must be done as the last thing in the loop)
        time1 = time2;
        clock_gettime(CLOCK_REALTIME, &time2);


    }
    return 0;
}
