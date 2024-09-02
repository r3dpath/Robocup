import socket
import serial
import threading

# Configuration parameters
TCP_IP = '192.168.4.1'   # The IP address to connect to
TCP_PORT = 8881        # The port to connect to
BUFFER_SIZE = 1024     # Buffer size for TCP communication

SERIAL_PORT = '/dev/ttys007'  # Serial port name (e.g., '/dev/tty.usbserial')
BAUD_RATE = 115200                    # Baud rate for serial communication

# Function to handle TCP to Serial communication
def tcp_to_serial(tcp_sock, ser):
    while True:
        data = tcp_sock.recv(BUFFER_SIZE)
        if not data:
            break
        ser.write(data)

# Function to handle Serial to TCP communication
def serial_to_tcp(ser, tcp_sock):
    while True:
        data = ser.read(BUFFER_SIZE)
        if data:
            print(data)
            tcp_sock.sendall(data)

def main():
    # Set up TCP socket
    tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_sock.connect((TCP_IP, TCP_PORT))
    
    # Set up serial port
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    
    # Start threads for bidirectional communication
    tcp_to_serial_thread = threading.Thread(target=tcp_to_serial, args=(tcp_sock, ser))
    serial_to_tcp_thread = threading.Thread(target=serial_to_tcp, args=(ser, tcp_sock))
    
    tcp_to_serial_thread.start()
    serial_to_tcp_thread.start()
    
    tcp_to_serial_thread.join()
    serial_to_tcp_thread.join()

if __name__ == "__main__":
    main()
