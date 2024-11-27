import tkinter as tk
import math
import numpy as np
import threading
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8880  # The port used by the server
s.connect((HOST, PORT))

def calcs(distances):
    distances = [int(distance) for distance in distances]
    print(distances)
    # diffs1 = np.array(distances[0:5]) - np.array(distances[5:10])
    # diffs2 = np.array(distances[10:15]) - np.array(distances[15:20])
    # diffs = np.concatenate((diffs1, diffs2))
    # print(diffs)
    # print(f"Max diff: {max(diffs)}, greater than average by {(max(diffs)/np.mean(diffs)*100)-100:.2f}%, amounts to top average reduction of {max(diffs)/np.mean(np.concatenate((distances[0:5], distances[15:20])))*100:.2f} ")

class TOFVisualizer:
    def __init__(self, root, serial_port):
        self.root = root
        self.serial_port = serial_port
        self.canvas = tk.Canvas(root, width=600, height=900)
        self.canvas.pack()

        # Left sensor angles (-10 to 10 degrees from left-angled sensor at 80 degrees)
        self.right_top_angles = [90, 85, 80, 75, 70]  # Left sensor top
        self.right_bottom_angles = [91, 86, 81, 76, 71]  # Left sensor bottom

        # Right sensor angles (-10 to 10 degrees from right-angled sensor at 100 degrees)
        self.left_top_angles = [110, 105, 100, 95, 90]  # Adjusted by subtracting 10
        self.left_bottom_angles = [111, 106, 101, 96, 91]  # Adjusted by subtracting 10

        # Starting points for sensors (left at 200px, right at 400px, 50px clearance between)
        self.start_points = [
            (290, 650), (290, 650), (290, 650), (290, 650), (290, 650),  # Left sensor top
            (290, 650), (290, 650), (290, 650), (290, 650), (290, 650),  # Left sensor bottom
            (310, 650), (310, 650), (310, 650), (310, 650), (310, 650),  # Right sensor top
            (310, 650), (310, 650), (310, 650), (310, 650), (310, 650),   # Right sensor bottom
            (250, 650), (350, 650)
        ]

        # Initialize distances and line objects (20 lines for both sensors)
        self.tof_distances = [0] * 22
        self.lines = [None] * 22

        self.draw_square()

        self.serial_thread = threading.Thread(target=self.read_serial)
        self.serial_thread.daemon = True
        self.serial_thread.start()

    def draw_square(self):
        self.canvas.create_rectangle(250, 650, 350, 750)

    def parse_serial_input(self, data):
        for dp in data.splitlines():
            try:
                distances = dp.split(':')
            except:
                return
            if distances[0] == 'S':
                distances = distances[1:23]
                self.tof_distances = [int(distance)/3 for distance in distances]
                calcs(distances)
                self.visualize_distances()

    def visualize_distances(self):
        # Left sensor - top (red), bottom (orange)
        for i in range(5):
            # Left top
            start_x, start_y = self.start_points[i]
            angle_rad = math.radians(self.left_top_angles[i])
            end_x = start_x + self.tof_distances[i] * math.cos(angle_rad)
            end_y = start_y - self.tof_distances[i] * math.sin(angle_rad)

            if self.lines[i]:
                self.canvas.delete(self.lines[i])
            self.lines[i] = self.canvas.create_line(start_x, start_y, end_x, end_y, fill='red')

            # Left bottom
            start_x, start_y = self.start_points[i+5]
            angle_rad = math.radians(self.left_bottom_angles[i])
            end_x = start_x + self.tof_distances[i+5] * math.cos(angle_rad)
            end_y = start_y - self.tof_distances[i+5] * math.sin(angle_rad)

            if self.lines[i+5]:
                self.canvas.delete(self.lines[i+5])
            self.lines[i+5] = self.canvas.create_line(start_x, start_y, end_x, end_y, fill='orange')

        # Right sensor - top (green), bottom (blue)
        for i in range(5):
            # Right top
            start_x, start_y = self.start_points[i+10]
            angle_rad = math.radians(self.right_top_angles[i])
            end_x = start_x + self.tof_distances[i+10] * math.cos(angle_rad)
            end_y = start_y - self.tof_distances[i+10] * math.sin(angle_rad)

            if self.lines[i+10]:
                self.canvas.delete(self.lines[i+10])
            self.lines[i+10] = self.canvas.create_line(start_x, start_y, end_x, end_y, fill='green')

            # Right bottom
            start_x, start_y = self.start_points[i+15]
            angle_rad = math.radians(self.right_bottom_angles[i])
            end_x = start_x + self.tof_distances[i+15] * math.cos(angle_rad)
            end_y = start_y - self.tof_distances[i+15] * math.sin(angle_rad)

            if self.lines[i+15]:
                self.canvas.delete(self.lines[i+15])
            self.lines[i+15] = self.canvas.create_line(start_x, start_y, end_x, end_y, fill='blue')

        for i in range(2):
            # Center sensors
            start_x, start_y = self.start_points[i+20]
            angle_rad = np.deg2rad(135) if i == 0 else np.deg2rad(45)
            end_x = start_x + self.tof_distances[i+20] * math.cos(angle_rad)
            end_y = start_y - self.tof_distances[i+20] * math.sin(angle_rad)

            if self.lines[i+20]:
                self.canvas.delete(self.lines[i+20])
            self.lines[i+20] = self.canvas.create_line(start_x, start_y, end_x, end_y, fill='purple')

    def read_serial(self):
        '''
        with serial.Serial(self.serial_port, 9600, timeout=1) as ser:
            while True:
                line = ser.readline().decode('utf-8').strip()
                if line:
                    self.parse_serial_input(line)
                    self.root.update_idletasks()
                    self.root.update()
        '''
        while True:
            data = s.recv(1024)
            if data:
                self.parse_serial_input(data.decode('utf-8'))
                self.root.update_idletasks()
                self.root.update()

def run_gui():
    root = tk.Tk()
    serial_port = '/dev/tty.usbmodem76988001'  # Change to your serial port
    visualizer = TOFVisualizer(root, serial_port)

    root.mainloop()

if __name__ == "__main__":
    run_gui()
