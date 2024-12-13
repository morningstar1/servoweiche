#!/usr/bin/python3

#from pymodbus.client import ModbusSerialClient
from crccheck.crc import Crc16CcittFalse
from time import sleep

import time
import serial

with serial.Serial('/dev/ttyUSB0', 115200, timeout=1) as ser:
    data = bytearray(8)
    data[0] = 5 # slave addr
    data[1] = 2 # function 1 read 2 write
    data[2] = 2 # register
    data[3] = 0 # register
    data[4] = 0xf # data
    data[5] = 0 # data
    
    crcinst = Crc16CcittFalse()
    crcinst.process(data[0:6])
    crcbytes = crcinst.finalbytes()
    data[6] = crcbytes[1]
    data[7] = crcbytes[0]
    print(":".join("{:02X}".format(c) for c in data))
    ser.write(data)
    s = ser.read(8)        # read up to ten bytes (timeout)
    
    if len(s) == 8:
        print(":".join("{:02X}".format(c) for c in s))
        crcinst = Crc16CcittFalse()
        crcinst.process(s[0:6])
        crcbytes = crcinst.finalbytes()
        if s[6] == crcbytes[1] and s[7] == crcbytes[0]:
            print("good")
        else:
            print("bad")
    else:
        print("not enought bytes")

    #print(s.decode('ascii'))

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

def writeData(register, leddata):
    data = bytearray(8)
    data[0] = 5 # slave addr
    data[1] = 2 # function 1 read 2 write
    data[2] = register # register
    data[3] = 0 # register
    data[4] = leddata # data
    data[5] = 0 # data
    
    crcinst = Crc16CcittFalse()
    crcinst.process(data[0:6])
    crcbytes = crcinst.finalbytes()
    data[6] = crcbytes[1]
    data[7] = crcbytes[0]
    print(":".join("{:02X}".format(c) for c in data))
    ser.write(data)
    s = ser.read(8)        # read up to ten bytes (timeout)
    
    if len(s) == 8:
        print(":".join("{:02X}".format(c) for c in s))
        crcinst = Crc16CcittFalse()
        crcinst.process(s[0:6])
        crcbytes = crcinst.finalbytes()
        if s[6] == crcbytes[1] and s[7] == crcbytes[0]:
            print("good")
        else:
            print("bad")
    else:
        print("not enought bytes")

    #print(s.decode('ascii'))

def loopServo():
    data = 1
    while(1):
        writeData(5, data % 8)
        data += 1
        sleep(5)

def loopLed():
    data = 1
    while(1):
        writeData(2, data % 0x8)
        data += 1
        sleep(0.5)

loopServo()
