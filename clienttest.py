#!/usr/bin/python3

from crccheck.crc import Crc16CcittFalse
from time import sleep

import time
import serial

ser = serial.Serial('/dev/ttyACM1', 115200, timeout=1)
#ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

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
    s = ser.read(8)
    
    if len(s) == 8:
        print(":".join("{:02X}".format(c) for c in s))
        crcinst = Crc16CcittFalse()
        crcinst.process(s[0:6])
        crcbytes = crcinst.finalbytes()
        if s[6] == crcbytes[1] and s[7] == crcbytes[0]:
            print("Ok")
        else:
            print("Fehler")
    else:
        print("nicht genügend Daten gelesen")
    return s[4] + s[5]*256

def writeData(register, leddata):
    sendFunction(2, register, leddata)

def readData(register):
    return sendFunction(1, register, 0)

def readDebug(register):
    return sendFunction(4, register, 0)

def relais(nr, direction, value):
    sendFunction(5, 0, nr | direction << 1 | value << 2)

# Modulfunktion
# 1 = 4x Weiche
# 2 = Kreuzweiche 2x
# 3 = 2x Weiche plue Kreuzweiche
# 4 = Doppelkreuzweiche
writeData(0, 1) # 4x Weiche
writeData(1, 0) # letzer Zustand Funktion 1
writeData(2, 0) # letzer Zustand Funktion 2
writeData(3, 0) # letzer Zustand Funktion 3
writeData(4, 0) # letzer Zustand Funktion 4
writeData(5, 10*200) # min Servo 1
writeData(6, 10*200) # min Servo 2
writeData(7, 10*200) # min Servo 3
writeData(8, 10*200) # min Servo 4
writeData( 9, 20*200) # max Servo 1
writeData(10, 20*200) # max Servo 2
writeData(11, 20*200) # max Servo 3
writeData(12, 20*200) # max Servo 4
writeData(13, 0) # Richtungsumkehr Servo 1
writeData(14, 0) # Richtungsumkehr Servo 2
writeData(15, 0) # Richtungsumkehr Servo 3
writeData(16, 0) # Richtungsumkehr Servo 4
writeData(17, 0) # servo speed
writeData(18, 2000) # servo step
#sendFunction(3, 0, 0) # reset

#readDebug(0)

