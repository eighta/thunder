from serial import Serial
# import bluetooth
from evdev import ecodes, ff, InputDevice
import asyncio
from crccheck.crc import Crcc16Mcrf4xx
import struct

def buildFrame(commandAndData):		
	delimiter = bytes([126])
	crc = Crcc16Mcrf4xx.calc(commandAndData)
	crcHex = struct.pack("<H",crc)
	return delimiter + commandAndData + crcHex + delimiter	

async def ds_listener(dev):	
	async for event in dev.async_read_loop():
		if event.type != ecodes.EV_SYN:
			notify_event(event.type,event.code,event.value)

def notify_event(type,code,value):
	# DualShock "Right Trigger"
	if type == 3 and code == 5:
		print(type,code,value)
		send_event(value)

def send_event(value):
	COMMAND_CUSTOM = 5
	commandByte = bytearray(struct.pack("B", COMMAND_CUSTOM))	
	dataByteArray = value.to_bytes(1,"little")	
	command = commandByte + dataByteArray
	frame = buildFrame(command)
	#bluetooth_client.send(bytes([value]))
	bluetoothSerial.write(frame)
	
if __name__ == "__main__":
	# arduino bluetooth connection
	print("Attempting connect against Arduino...")
	
	'''
	# Doing Bluetooth connection (fisica)
	bd_addr = "D7:02:04:0F:47:74"
	port = 1
	bluetooth_client=bluetooth.BluetoothSocket(bluetooth.RFCOMM)
	bluetooth_client.connect((bd_addr, port))'''
	
	# Basandose en un archivo
	# sudo rfcomm connect hci0 D7:02:04:0F:47:74 1
	bluetoothSerial = Serial("/dev/rfcomm0", baudrate=9600)

	# test data	
	#send_event(200)

	print("Connection Established\n")
	
	print("Attempting adquire access to DualShock gamepad...")
	dsDevice = InputDevice("/dev/input/event2")
	
	tasks = [asyncio.ensure_future(ds_listener(dsDevice))]
	print("Listening DualShock events\n")	

	loop = asyncio.get_event_loop()
	loop.run_until_complete(asyncio.gather(*tasks))