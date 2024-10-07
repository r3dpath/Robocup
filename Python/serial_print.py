import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8880  # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    while(True):
        data = s.recv(1024)
        if data:
            x = data.decode().strip()
            try:
                x = int(x)
                print(x)
            except:
                pass
            # try:
            #     sections = x.split(":")
            #     if sections[0] == "S":
            #         sections = sections[1:]
            #         sections = [int(distance) for distance in sections]
            #         print(f"Top Left: \t{sections[0:5]}")
            #         print(f"Bottom Left: \t{sections[5:10]}")
            #         print(f"Top Right: \t{sections[10:15]}")
            #         print(f"Bottom Right: \t{sections[15:20]}")
            # except:
            #     pass