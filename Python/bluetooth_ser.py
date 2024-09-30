import asyncio
from bleak import BleakClient
import socket

# UUID of the service and characteristic you want to connect to
SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
CHARACTERISTIC_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  # Example UUID for read characteristic

ADDRESS = "DB4A242F-16FA-0EB3-8F58-40F17BB027BB"

HOST = "127.0.0.1"
PORT = 8880

connected_clients = []
RECONNECT_INTERVAL = 5  # Time in seconds between reconnection attempts

async def find_device():
    return ADDRESS

async def handle_client(client_socket, client_address):
    global connected_clients
    try:
        connected_clients.append(client_socket)
        print(f"Client {client_address} connected. Total clients: {len(connected_clients)}")

        while True:
            data = await asyncio.to_thread(client_socket.recv, 1024)
            if not data:
                break
            print(f"Received {data} from {client_address}")

    except Exception as e:
        print(f"Error with client {client_address}: {e}")
    finally:
        connected_clients.remove(client_socket)
        client_socket.close()
        print(f"Connection closed for {client_address}. Total clients: {len(connected_clients)}")

async def notify_clients(data):
    global connected_clients
    for client_socket in connected_clients:
        try:
            print(f"Sending data to client: {data}")
            client_socket.sendall(data)
        except Exception as e:
            print(f"Error sending data to client: {e}")

async def read_bluetooth_data(client):
    buf = bytearray()

    def callback(sender, data):
        buf.extend(data)
        if data[-1] == 10:  # Newline detected
            print(f"Broadcasting data: {buf}")
            asyncio.create_task(notify_clients(buf.copy()))
            buf.clear()

    await client.start_notify(CHARACTERISTIC_UUID, callback)
    print("Notification started")

    while True:
        # Keep checking if the client is still connected
        if not client.is_connected:
            print("Bluetooth device disconnected during read.")
            break
        await asyncio.sleep(1)

async def connect_bluetooth():
    addr = await find_device()
    if not addr:
        print("Could not find the device.")
        return None

    while True:
        try:
            print(f"Attempting to connect to {addr}...")
            client = BleakClient(addr)
            await client.connect()
            print("Connected to Bluetooth device.")
            return client
        except Exception as e:
            print(f"Failed to connect: {e}")
            print(f"Reattempting connection in {RECONNECT_INTERVAL} seconds...")
            await asyncio.sleep(RECONNECT_INTERVAL)

async def manage_bluetooth_connection():
    client = None

    while True:
        if client is None or not client.is_connected:
            client = await connect_bluetooth()
        
        if client and client.is_connected:
            try:
                await read_bluetooth_data(client)
            except Exception as e:
                print(f"Bluetooth error: {e}")
            finally:
                if client.is_connected:
                    await client.disconnect()
                print("Bluetooth disconnected. Retrying connection...")

        # Add a small sleep to avoid rapid retries
        await asyncio.sleep(RECONNECT_INTERVAL)

async def server():
    asyncio.create_task(manage_bluetooth_connection())

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        print(f"Server listening on {HOST}:{PORT}")

        while True:
            client_socket, client_address = await asyncio.to_thread(s.accept)
            asyncio.create_task(handle_client(client_socket, client_address))

if __name__ == "__main__":
    asyncio.run(server())
