import argparse
import serial
from serial.tools.list_ports import comports
#from serial.tools import hexlify_codec
import time

argparser = argparse.ArgumentParser()
argparser.add_argument("port")
argparser.add_argument("--baud", type=int, default=115200)
args = argparser.parse_args()

ser = serial.Serial(args.port, args.baud)
print("Using port {}".format(ser.name))


# Send an integer every second
for i in range(0,60):
    d = bytearray()
    d.append(i)
    print("{} ".format(i))
    ser.write(d)
#    ser.write(b"\n")
    time.sleep(1)


ser.close()
