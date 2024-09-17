import socket
import datetime
import asyncio

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8880  # The port used by the server

connected_clients = []

def record():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    start = datetime.datetime.now()
    
    with open('record.txt', 'w') as f:
        for _ in range(100):
            data = read_socket(s)
            elapsed_time = (datetime.datetime.now() - start).microseconds
            f.write(f"{elapsed_time}-{data.decode('utf-8')}\r\n")

async def replay():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen()
    print(f"Server listening on {HOST}:{PORT}")

    # Start the data serving task
    asyncio.create_task(serve())

    # Continuously accept and handle new clients
    while True:
        client_socket, client_address = await asyncio.to_thread(server_socket.accept)
        asyncio.create_task(handle_client(client_socket, client_address))

async def handle_client(client_socket, client_address):
    global connected_clients
    connected_clients.append(client_socket)
    print(f"Client {client_address} connected. Total clients: {len(connected_clients)}")

    try:
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

async def serve():
    global connected_clients

    while True:
        with open('record.txt', 'r') as f:
            start = datetime.datetime.now()
            for line in f.readlines():

                elapsed_time, data = line.split('-')
                '''
                elapsed_time = int(elapsed_time.strip())

                while (datetime.datetime.now() - start).microseconds < elapsed_time:
                    await asyncio.sleep(0.001)  # Sleep briefly to prevent tight loop
                
                '''
                await asyncio.sleep(0.5)

                for client_socket in connected_clients:
                    try:
                        print(f"Sending data to client: {bytearray(data.encode())}")
                        await asyncio.to_thread(client_socket.sendall, bytearray(data.encode()))
                    except Exception as e:
                        print(f"Error sending data to client: {e}")

def read_socket(s):
    data = s.recv(1024)
    if data:
        print(data.decode('utf-8'))
    return data

if __name__ == "__main__":
    asyncio.run(replay())
    #record()
