import tkinter as tk
import math
import unittest
import threading
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8880  # The port used by the server
s.connect((HOST, PORT))

class TOFVisualizer:
    def __init__(self, root, serial_port):
        self.root = root
        self.serial_port = serial_port
        self.canvas = tk.Canvas(root, width=600, height=900)
        self.canvas.pack()

        self.tof_angles = [90+15, 90+7.5, 90, 90-7.5, 90-15, 90+16, 90+8.5, 90+1, 90-8.5, 90-16, 90+30, 90-30, 270]  # Default start angles for the TOF sensors
        self.tof_distances = [0] * 13

        self.start_points = [(300, 650), (300, 650), (300, 650), (300, 650), (300, 650), (300, 650), (300, 650), (300, 650), (300, 650), (300, 650), (280, 650), (320, 650), (300, 750)]  # Starting points for each TOF sensor
        self.lines = [None] * 13

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
                distances = distances[1:11]
                self.tof_distances[:10] = [int(distance)/3 for distance in distances]
                self.visualize_distances()
            elif distances[0] == 'T':
                distances = distances[1:4]
                self.tof_distances[10:] = [int(distance)/3 for distance in distances]
            elif distances[0] == 'X':
                print(dp)

    def visualize_distances(self):
        for i, (start_x, start_y) in enumerate(self.start_points):
            colour = 'red' if i < 5 else 'green'
            angle_rad = math.radians(self.tof_angles[i])
            end_x = start_x + self.tof_distances[i] * math.cos(angle_rad)
            end_y = start_y - self.tof_distances[i] * math.sin(angle_rad)

            if self.lines[i]:
                self.canvas.delete(self.lines[i])
            self.lines[i] = self.canvas.create_line(start_x, start_y, end_x, end_y, fill=colour)

    def visualize_longrange(self, distance):
        start_x, start_y = (300, 700)
        angle_rad = math.radians(90)
        end_x = start_x + distance * math.cos(angle_rad)
        end_y = start_y - distance * math.sin(angle_rad)
        if self.lines[11]:
            self.canvas.delete(self.lines[11])
        self.lines[11] = self.canvas.create_line(start_x, start_y, end_x, end_y, fill='blue')

    def set_angle(self, index, angle):
        self.tof_angles[index] = angle
        self.visualize_distances()

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

    def update_angle_0(val):
        visualizer.set_angle(0, int(val))
    
    def update_angle_1(val):
        visualizer.set_angle(1, int(val))

    def update_angle_2(val):
        visualizer.set_angle(2, int(val))

    def update_angle_3(val):
        visualizer.set_angle(3, int(val))

    # tk.Scale(root, from_=0, to=360, label='Sensor 0 Angle', orient=tk.HORIZONTAL, command=update_angle_0).pack()
    # tk.Scale(root, from_=0, to=360, label='Sensor 1 Angle', orient=tk.HORIZONTAL, command=update_angle_1).pack()
    # tk.Scale(root, from_=0, to=360, label='Sensor 2 Angle', orient=tk.HORIZONTAL, command=update_angle_2).pack()
    # tk.Scale(root, from_=0, to=360, label='Sensor 3 Angle', orient=tk.HORIZONTAL, command=update_angle_3).pack()

    root.mainloop()

class TestTOFVisualizer(unittest.TestCase):
    def setUp(self):
        self.root = tk.Tk()
        self.visualizer = TOFVisualizer(self.root, 'COM3')  # Change to your serial port

    def test_parse_serial_input(self):
        self.visualizer.parse_serial_input("111:222:333:444")
        self.assertEqual(self.visualizer.tof_distances, [111, 222, 333, 444])

    def test_visualize_distances(self):
        self.visualizer.parse_serial_input("111:222:333:444")
        # Additional logic to verify line positions can be implemented here

    def tearDown(self):
        self.root.destroy()

if __name__ == "__main__":
    run_gui()