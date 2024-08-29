import asyncio
from bleak import BleakClient, BleakScanner
import socket
import os

# UUID of the service and characteristic you want to connect to
SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
CHARACTERISTIC_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  # Example UUID for read characteristic

ADDRESS = "DB4A242F-16FA-0EB3-8F58-40F17BB027BB"

HOST = "127.0.0.1"
PORT = 8880

# Define the serial port to bind
serial_port = "/dev/tty.Bluetooth-Incoming-Port"

async def find_device():
    '''print("Scanning for devices...")
    device = await BleakScanner.find_device_by_name("mpy-uart")
    if device:
        print(f"Found {device.name} - {device.address}")
        return device.address
    else:
        print("No device found.")
        return None'''
    
    return ADDRESS
    '''
    for device in devices:
        print(f"Found {device.name} - {device.address}")
        if SERVICE_UUID.lower() in [str(uuid).lower() for uuid in device.metadata['uuids']]:
            print(f"Found matching service on {device.address}")
            return device.address

    print("No device with the desired service found.")
    return None'''

async def connect_and_bind(addr):
    buf = bytearray()
    async with BleakClient(addr) as client:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind((HOST, PORT))
            s.listen()
            conn, addr = s.accept()
            print(f"Connected to {addr}")

            def callback(sender, data):
                print(f"Received data from {sender}: {data}")
                buf.extend(data)
                if data[-1] == 10:
                    print(buf)
                    conn.sendall(buf)
                    buf.clear()
                

            await client.start_notify(CHARACTERISTIC_UUID, callback)
            print("Notification started")

            # Keep the connection open to receive data
            await asyncio.Future()  # Adjust as needed

            await client.stop_notify(CHARACTERISTIC_UUID)
            print("Notification stopped")

if __name__ == "__main__":
    addr = asyncio.run(find_device())
    if addr:
        asyncio.run(connect_and_bind(addr))
    else:
        print("Could not find the device.")
