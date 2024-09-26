import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8880  # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    means=[0, 0, 0, 0, 0]
    n=0
    while(True):
        data = s.recv(1024)
        if data:
            n+=1
            line = data.decode().strip()
            vals = line.split(':')
            diffs=[]
            for idx, val in enumerate(vals[:5]):
                means[idx] -= means[idx] / n
                means[idx] += (int(val)-int(vals[int(idx+5)])) / n
            # print the values in means to 2 decimal places
            print([round(mean, 2) for mean in means])