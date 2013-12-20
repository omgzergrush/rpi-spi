/***********************************************************************
 * adis16485spiTest.cpp. Sample program that tests the adis16485spi class.
 * an adis16485spi class object (a2d) is created. the a2d object is instantiated
 * using the overloaded constructor. which opens the spidev0.0 device with
 * SPI_MODE_0 (MODE 0) (defined in linux/spi/spidev.h), speed = 1MHz &
 * bitsPerWord=8.
 *
 * call the spiWriteRead function on the a2d object 20 times. Each time make sure
 * that conversion is configured for single ended conversion on CH0
 * i.e. transmit ->  byte1 = 0b00000001 (start bit)
 *                   byte2 = 0b1000000  (SGL/DIF = 1, D2=D1=D0=0)
 *                   byte3 = 0b00000000  (Don't care)
 *      receive  ->  byte1 = junk
 *                   byte2 = junk + b8 + b9
 *                   byte3 = b7 - b0
 *    
 * after conversion must merge data[1] and data[2] to get final result
 *
 *
 *
 * *********************************************************************/
#include "adis16485spi.h"

using namespace std;

int main(void)
{
    adis16485spi a2d("/dev/spidev0.0", SPI_MODE_3, 15000000, 8);
    
    unsigned char data[2];

    while(true)
    {
        data[0] = 0x7E; //  first byte transmitted 
        data[1] = 0x00; // second byte transmitted

        a2d.spiWriteRead(data, sizeof(data) );

        int pid = 0;
        pid = (data[0]<< 8) & 0xffff;
        //merge data[1] & data[2] to get result
        pid |=  (data[1] & 0xff);
        sleep(1);
        cout << "The Result is: " << pid << endl;
    }
    return 0;
}
