#!/usr/bin/python3

from pymodbus.client import ModbusSerialClient
import time

def run():
    client = ModbusSerialClient(port="/dev/ttyUSB0",)

    client.connect()
    dsr = 1
    state = 0
    while(True):
        #input("Press the Enter key to continue: ")
        print("write some data " + str(state % 8))
        client.write_register(address=dsr%16, value=state%8, slave=0x01)
        state += 1;
        dsr += 1
        time.sleep(1)
    #...
    client.close()
run()
print("end")
