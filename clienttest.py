#!/usr/bin/python3

#from pymodbus.client import ModbusSerialClient
from crccheck.crc import Crc16CcittFalse
from time import sleep

import time
import serial

#ser = serial.Serial('/dev/ttyACM1', 115200, timeout=1)
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

def sendFunction(function, register, senddata):
    data = bytearray(8)
    data[0] = function
    data[1] = 0
    data[2] = register # register
    data[3] = 0 # register
    data[4] = senddata & 0xFF # data
    data[5] = senddata >> 8 # data
    
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
    return s[4] + s[5]*256

def writeData(register, leddata):
    sendFunction(2, register, leddata)

def readData(register):
    return sendFunction(1, register, 0)

def readDebug(register):
    return sendFunction(4, register, 0)
def relais(nr, direction, value):
    sendFunction(5, 0, nr | direction << 1 | value << 2)


#readData(0)
#readData(1)
#readData(2)
#readData(3)
#readData(4)

#writeData(0, 4)

#writeData(0, 1)
#writeData(1, 0) # switch
#writeData(2, 0) # switch
#writeData(3, 0) # switch
#writeData(4, 0) # switch
#writeData(5, 10*200) # min
#writeData(6, 10*200) # min
#writeData(7, 10*200) # min
#writeData(8, 10*200) # min
#writeData( 9, 20*200) # max
#writeData(10, 20*200) # max
#writeData(11, 20*200) # max
#writeData(12, 20*200) # max
#writeData(13, 0) # switch
#writeData(14, 0) # switch
#writeData(15, 0) # switch
#writeData(16, 0) # switch
#writeData(17, 0) # servo speed
#writeData(18, 2000) # servo step
#sendFunction(3, 0, 0) # reset

#readDebug(0)

