import serial
import time
arduino = serial.Serial('/dev/ttyUSB1', 115200, timeout=.1) #edit port to match your port/OS
time.sleep(1) #give the connection a second to settle
arduino.write(b"Hello from Python!")
while True:
	command = "/"+input("Enter command")+";"
	print(command.encode())
	arduino.write(command.encode())
	time.sleep(0.1)
	data = arduino.readline()
	data = arduino.readline()
	data = arduino.readline()
	if data:
		print(data)
