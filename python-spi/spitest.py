#!/usr/bin/python
import spidev
import time
from datetime import datetime 

#open SPI bus
spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 3
spi.max_speed_hz = 15000000
spi.bits_per_word = 8
print "bits: ", spi.bits_per_word
print "mode: ", spi.mode
print "speed: ", spi.max_speed_hz

# Function to read SPI data 
def ReadData(send):
    data = spi.xfer2(send)
    return data

def combine(a, b):
    return (a << 8) | b

def twos_comp(val, bits):
    if( (val&(1<<(bits-1))) != 0):
        val = val - (1<<bits)
    return val

def gyro_out(gyro):
        gyro = int(gyro, 0)
        gyro = twos_comp(gyro, 16)
        return gyro * 0.02

def acc_out(acc):
    acc= int(acc, 0)
    acc= twos_comp(acc, 16)
    return acc * 0.25


# Define delay between readings
delay = 0.5 

#main loop
try:
    begin = datetime.now()
    ReadData([0x7E, 0x00])
    product_id = ReadData([0x12, 0x00])
    end = datetime.now()
    print "Delay test: ", (end.microsecond - begin.microsecond), " us"
    pid_hex = hex(combine(product_id[0], product_id[1])) 

    # Print out results
    print "--------------------------------------------"
    print "Product id (should be 0x4065): ", pid_hex

    while True:
        xgyro = ReadData([0x16, 0x00])
        xgyro = hex(combine(xgyro[0], xgyro[1])) 
        xgyro = gyro_out(xgyro) 

        ygyro = ReadData([0x1A, 0x00])
        ygyro = hex(combine(ygyro[0], ygyro[1]))
        ygyro = gyro_out(ygyro)
        
        zgyro = ReadData([0x1E, 0x00])
        zgyro = hex(combine(zgyro[0], zgyro[1]))
        zgyro = gyro_out(zgyro)
        
        xacc = ReadData([0x22, 0x00])
        xacc = hex(combine(xacc[0], xacc[1]))
        xacc = acc_out(xacc)

        yacc = ReadData([0x26, 0x00])
        yacc = hex(combine(yacc[0], yacc[1]))
        yacc = acc_out(yacc)

        zacc = ReadData([0x0E, 0x00])
        zacc = hex(combine(zacc[0], zacc[1]))
        zacc = acc_out(zacc)

        temp_raw = ReadData([0x12, 0x00])
        temp = hex(combine(temp_raw[0], temp_raw[1]))
        temp = int(temp, 0)
        temp = 0.00565 * temp + 25

        # Print out results
        print "--------------------------------------------"
        print "Temp:    ", "{:10.4f}".format(temp), " C"
        print "Xgyro:   ", "{:10.4f}".format(xgyro), " deg/s"
        print "Ygyro:   ", "{:10.4f}".format(ygyro), " deg/s"
        print "Zgyro:   ", "{:10.4f}".format(zgyro), " deg/s"
        print "Xacc:    ", "{:10.4f}".format(xacc), " mg"
        print "Yacc:    ", "{:10.4f}".format(yacc), " mg"
        print "Zacc:    ", "{:10.4f}".format(zacc), " mg"

        # Wait before repeating loop
        time.sleep(delay)

except KeyboardInterrupt: #Ctrl+C pressed
    spi.close()
#end try
