import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8880  # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    while(True):
        data = s.recv(1024)
        if data:
            try:
                line = data.decode().strip()
                vals = line.split(':')
                vals = [int(val) for val in vals]
                print([val - (vals[int(idx+5)]-20) for idx, val in enumerate(vals[:5])])
            except:
                pass
            