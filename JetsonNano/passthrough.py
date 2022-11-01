from ublox_gps import UbloxGps
import serial
import socket
import time
from multiprocessing import Process, Queue


# Allow serial interface to become initialized
time.sleep(15)

static_ip_server = '192.168.1.100'

arduino_port = '/dev/ttyACM0'
arduino_baud = 57600

gps_port = '/dev/ttyACM1'
gps_baud = 38400

to = 60 

msg_queue = Queue()


def snoop_arduino():
	arduino_conn = serial.Serial(arduino_port, arduino_baud, timeout=to)
	
	while True:
		try:
			if arduino_conn.in_waiting > 0:
				line = arduino_conn.readline()
				msg_queue.put(line)

		except (ValueError, IOError) as err:
			print(err)


def snoop_gps():
	gps_connection = serial.Serial(gps_port, gps_baud, timeout=to)
	gps = UbloxGps(gps_connection)
	
	while True:
		try:
			coords = gps.geo_coords()
			gps_location = 'Lon: ' + str(coords.lon) + ', Lat: ' + str(coords.lat)
			msg_queue.put(gps_location.encode('utf-8'))

		except (ValueError, IOError) as err:
			print(err) 



def main():
	# Set up connection with socket
	address = (static_ip_server, 12000)
	clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	clientSocket.settimeout(20)

	# Initialize processes 
	arduino_proc = Process(target=snoop_arduino)
	gps_proc = Process(target=snoop_gps)

	# Start processes
	arduino_proc.start()
	gps_proc.start()

	while True:
		try:
			clientSocket.sendto(msg_queue.get(), address)

		except (ValueError, IOError) as err:
			print(err)
			

if __name__ == '__main__':
	main()
