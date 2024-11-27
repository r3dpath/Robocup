import tkinter as tk
import threading
import socket
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8880  # The port used by the server
s.connect((HOST, PORT))

class CoordinatePlotter:
    def __init__(self, root):
        self.root = root

        # Setup the matplotlib figure to display the 2.4x4.9m box
        self.fig, self.ax = plt.subplots(figsize=(4, 10), tight_layout=True)   
        self.canvas_plot = FigureCanvasTkAgg(self.fig, master=root)
        self.canvas_plot.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)

        # Define the dimensions of the box (in meters) and scaling factor (1000 mm = 1m)
        self.box_width_mm = 2400  # 2.4 meters = 2400 mm
        self.box_height_mm = 4900  # 4.9 meters = 4900 mm

        # Setup the plot for a 2.4x4.9m area with a 300mm offset
        self.ax.set_xlim(0, self.box_width_mm)
        self.ax.set_ylim(0, self.box_height_mm)

        self.ax.set_xlabel("X (mm)")
        self.ax.set_ylabel("Y (mm)")
        self.ax.set_aspect('equal')
        self.ax.grid(True)
        

        # Line plot that will update with new points
        self.points, = self.ax.plot([], [], 'ro')  # Red point for the current location
        self.x_vals = []
        self.y_vals = []

        self.targets, = self.ax.plot([], [], 'bo')  # Blue point for the target location
        self.x_target = []
        self.y_target = []

        # Thread to handle reading data in the background
        self.serial_thread = threading.Thread(target=self.read_data)
        self.serial_thread.daemon = True
        self.serial_thread.start()

    def update_plot(self):
        """Update the plot with new points."""
        self.points.set_data(self.x_vals, self.y_vals)  # Update points
        self.targets.set_data(self.x_target, self.y_target)  # Update target points
        self.ax.relim()  # Recalculate limits
        self.ax.autoscale_view()  # Rescale the view
        self.canvas_plot.draw()  # Redraw the canvas

    def plot_point(self, x, y):
        """Add the point to the list of plotted points and update the plot."""
        self.x_vals.append(x)
        self.y_vals.append(y)


        # Optionally limit to the last 50 points for better clarity
        if len(self.x_vals) > 500:
            self.x_vals.pop(0)
            self.y_vals.pop(0)

        self.update_plot()

    def plot_target(self, x, y):
        """Add the point to the list of plotted points and update the plot."""
        self.x_target.append(x)
        self.y_target.append(y)

        # Optionally limit to the last 50 points for better clarity
        if len(self.x_target) > 2:
            self.x_target.pop(0)
            self.y_target.pop(0)

        self.update_plot()

    def parse_input(self, data):
        """Parse the incoming data in the form of P:x:y."""
        for dp in data.splitlines():
            if dp.startswith("P"):
                try:
                    _, x_str, y_str = dp.split(":")
                    x = float(x_str)
                    y = float(y_str)
                    print(f"Received point: ({x}, {y})")

                    # Only plot the point if it's within the boundaries
                    #if 0 <= x <= self.box_width_mm and 0 <= y <= self.box_height_mm:
                    self.plot_point(x, y)

                except ValueError:
                    print("Invalid data format. Expected 'P:x:y'.")
            elif dp.startswith("T"):
                try:
                    _, x_str, y_str = dp.split(":")
                    x = float(x_str)
                    y = float(y_str)
                    print(f"Received target: ({x}, {y})")

                    # Only plot the point if it's within the boundaries
                    #if 0 <= x <= self.box_width_mm and 0 <= y <= self.box_height_mm:
                    self.plot_target(x, y)

                except ValueError:
                    print("Invalid data format. Expected 'T:x:y'.")

    def read_data(self):
        """Read the incoming data from the socket."""
        while True:
            data = s.recv(1024)
            if data:
                self.parse_input(data.decode('utf-8'))
                self.root.update_idletasks()
                self.root.update()

def run_gui():
    root = tk.Tk()
    root.title("2.4x4.9m Room Coordinate Plotter")
    plotter = CoordinatePlotter(root)
    root.mainloop()

if __name__ == "__main__":
    run_gui()
