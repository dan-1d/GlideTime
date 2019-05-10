import argparse
import serial
from serial.tools.list_ports import comports
#from serial.tools import hexlify_codec
import time

argparser = argparse.ArgumentParser()
argparser.add_argument("port")
argparser.add_argument("--baud", type=int, default=115200)
argparser.add_argument("--seconds", type=int, default=6*60)
args = argparser.parse_args()

ser = serial.Serial(args.port, args.baud)
print("Using port {}".format(ser.name))
print("Waiting for start command")

#while( ser.readline() != "start" ):
#    pass

## Wait for start
secs = args.seconds
print(ser.readline()) # wait for start
ser.write(bytearray([42]))  #write magic number to acknowledge

#Pause for the calibration period
time.sleep(secs)

ser.write(secs)
print(secs);

#d = bytearray()
#for i in range(0, 2+ args.seconds):
#    d.append(i)
#    print("{} ".format(i))

#    ser.write(b"\n")
#   ser.write(d)
#    time.sleep(1)



ser.close()
