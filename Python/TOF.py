import tkinter as tk
import math
import unittest
import serial
import threading


class TOFVisualizer:
    def __init__(self, root, serial_port):
        self.root = root
        self.serial_port = serial_port
        self.canvas = tk.Canvas(root, width=600, height=700)
        self.canvas.pack()

        self.tof_angles = [90, 135, 45, 270]  # Default start angles for the TOF sensors
        self.tof_distances = [10, 10, 10, 10]

        self.start_points = [(300, 350), (250, 350), (350, 350), (300, 450)]  # Starting points for each TOF sensor
        self.lines = [None] * 4

        self.draw_square()

        self.serial_thread = threading.Thread(target=self.read_serial)
        self.serial_thread.daemon = True
        self.serial_thread.start()

    def draw_square(self):
        self.canvas.create_rectangle(250, 350, 350, 450)

    def parse_serial_input(self, data):
        try:
            distances = data.split(':')
        except:
            return
        self.tof_distances = [int(distance) for distance in distances]
        self.visualize_distances()

    def visualize_distances(self):
        for i, (start_x, start_y) in enumerate(self.start_points):
            angle_rad = math.radians(self.tof_angles[i])
            end_x = start_x + self.tof_distances[i] * math.cos(angle_rad)
            end_y = start_y - self.tof_distances[i] * math.sin(angle_rad)

            if self.lines[i]:
                self.canvas.delete(self.lines[i])
            self.lines[i] = self.canvas.create_line(start_x, start_y, end_x, end_y, fill='red')

    def set_angle(self, index, angle):
        self.tof_angles[index] = angle
        self.visualize_distances()

    def read_serial(self):
        x = 1
        with serial.Serial(self.serial_port, 9600, timeout=1) as ser:
            while True:
                line = ser.readline().decode('utf-8').strip()
                if line:
                    self.parse_serial_input(line)
                    self.root.update_idletasks()
                    self.root.update()
        # while True:
        #     self.parse_serial_input(f"{x}:{x}:{x}:{x}")
        #     x += 1
        #     self.root.update_idletasks()
        #     self.root.update()
        #     if x > 1000:
        #         x = 1

def run_gui():
    root = tk.Tk()
    serial_port = 'COM3'  # Change to your serial port
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