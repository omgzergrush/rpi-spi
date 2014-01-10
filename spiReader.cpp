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
#include "spiReader.h"

using namespace std;

SpiReader::SpiReader()
{    
    spiReadWriteErrorCode = 0;

    // Open SPI communication, ADIS16485 uses 16 bit communication
    // but it fails (bug in spi/spidev.h?) so we use 8 bit instead.
    adis16485spi a2d("/dev/spidev0.0", SPI_MODE_3, 15000000, 8);
    
    cout << fixed;
    cout.precision(3);
}

SpiReader::~SpiReader()
{
}

unsigned int SpiReader::readData(adis16485spi& a2d, unsigned char* data, int length)
{
        spiReadWriteErrorCode = a2d.spiWriteRead(data, length );
        unsigned int pid = 0;
        //merge data[1] & data[2] to get result
        //(presumes int >= 32 bits)
        pid = (data[0]<< 8) & 0xffff; 
        pid |=  (data[1] & 0xff);
        return pid;
}

double SpiReader::gyroOut(short int gyroRaw)
{
    return (double)gyroRaw * 0.02;
}

double SpiReader::accOut(short int accRaw)
{
    return (double)accRaw * 0.25;
}



int SpiReader::readSpi(void)
{
    data[0] = 0x16; 
    data[1] = 0x00;
    short int gyroRaw = 0;
    gyroRaw = readData(a2d, data, sizeof(data));
    if (spiReadWriteErrorCode < 0) return spiReadWriteErrorCode;
    inertia.xgyro = gyroOut(gyroRaw);

    data[0] = 0x1A; 
    data[1] = 0x00;
    gyroRaw = readData(a2d, data, sizeof(data));
    if (spiReadWriteErrorCode < 0) return spiReadWriteErrorCode;
    inertia.ygyro = gyroOut(gyroRaw);

    data[0] = 0x1E; 
    data[1] = 0x00;
    gyroRaw = readData(a2d, data, sizeof(data));
    if (spiReadWriteErrorCode < 0) return spiReadWriteErrorCode;
    inertia.zgyro = gyroOut(gyroRaw);

    data[0] = 0x22; 
    data[1] = 0x00;
    short int accRaw = 0;
    accRaw = readData(a2d, data, sizeof(data));
    if (spiReadWriteErrorCode < 0) return spiReadWriteErrorCode;
    inertia.xacc = accOut(accRaw);

    data[0] = 0x26; 
    data[1] = 0x00;
    accRaw = 0;
    accRaw = readData(a2d, data, sizeof(data));
    if (spiReadWriteErrorCode < 0) return spiReadWriteErrorCode;
    inertia.yacc = accOut(accRaw);

    data[0] = 0x12; 
    data[1] = 0x00;
    accRaw = 0;
    accRaw = readData(a2d, data, sizeof(data));
    if (spiReadWriteErrorCode < 0) return spiReadWriteErrorCode;
    inertia.zacc = accOut(accRaw);
    
    return 0;
}

const SpiReader::InertiaData& SpiReader::getAll(void) const 
{
    return inertia;
}

void SpiReader::printAll(void) const
{
    cout << "------------------------------------------" << endl;  
    cout << "Xgyro: " << inertia.xgyro << endl;
    cout << "Ygyro: " << inertia.ygyro << endl;
    cout << "Zgyro: " << inertia.zgyro << endl;
    cout << "Xacc:  " << inertia.xacc << endl;
    cout << "Yacc:  " << inertia.yacc << endl;
    cout << "Zacc:  " << inertia.zacc << endl;

}
