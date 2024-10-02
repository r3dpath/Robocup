import tkinter as tk
import numpy as np
import threading
import socket
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8880  # The port used by the server
s.connect((HOST, PORT))
num_samples = 100

class LineGraphVisualizer:
    def __init__(self, root):
        self.root = root
        self.data1 = [0] * num_samples  # Placeholder for graph 1
        self.data2 = [0] * num_samples  # Placeholder for graph 2
        self.data3 = [0] * num_samples  # Placeholder for graph 3

        # Set up matplotlib figure with 3 subplots (line graphs)
        self.fig, (self.ax1, self.ax2, self.ax3) = plt.subplots(3, 1, figsize=(10, 8))
        self.fig.dpi = 120
        self.fig.tight_layout()
        self.canvas_plot = FigureCanvasTkAgg(self.fig, master=root)
        self.canvas_plot.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)

        self.ax1.set_title("Absolute Max Difference")
        self.ax1.grid(True) 
        self.ax2.set_title("")
        self.ax2.grid(True)
        self.ax3.set_title("")
        self.ax3.grid(True)

        # Line objects for updating the plots
        self.line1, = self.ax1.plot(self.data1, label="Graph 1")
        self.line2, = self.ax2.plot(self.data2, label="Graph 2")
        self.line3, = self.ax3.plot(self.data3, label="Graph 3")

        # Set up threading to handle data in the background
        self.serial_thread = threading.Thread(target=self.read_data)
        self.serial_thread.daemon = True
        self.serial_thread.start()

    def update_graphs(self):
        """Update all three graphs with the latest data."""
        # Clear the previous lines and update with the new data
        self.line1.set_ydata(self.data1)
        self.line2.set_ydata(self.data2)
        self.line3.set_ydata(self.data3)

        # Adjust the axes to fit the new data
        self.ax1.relim()
        self.ax1.autoscale_view()

        self.ax2.relim()
        self.ax2.autoscale_view()

        self.ax3.relim()
        self.ax3.autoscale_view()

        # Redraw the canvas
        self.canvas_plot.draw()

    def dummy_process1(self, distances):
        """Dummy processing function for graph 1."""
        differences = np.array(distances[:5]) - np.array(distances[5:])
        return max(differences)  # Example: max difference between first 5 and last 5 distances

    def dummy_process2(self, distances):
        """Dummy processing function for graph 2."""
        differences = np.array(distances[:5]) - np.array(distances[5:])
        max_idx = np.argmax(differences)
        percentage = differences[max_idx] / distances[max_idx] * 100
        return percentage

    def dummy_process3(self, distances):
        """Dummy processing function for graph 3."""
        processed_value = np.sum(distances[:10])  # Example: sum of all distances
        return processed_value

    def parse_input(self, data):
        """Parse received data and update the graphs with processed values."""
        for dp in data.splitlines():
            if dp[0] != "S":
                return
            distances = [int(val) for val in dp.split(':')[1:11]]  # Example: 10 distances expected
            
            # Process data for each graph
            self.data1.append(self.dummy_process1(distances))
            self.data2.append(self.dummy_process2(distances))
            self.data3.append(self.dummy_process3(distances))

            # Keep only the last 50 values
            self.data1 = self.data1[-num_samples:]
            self.data2 = self.data2[-num_samples:]
            self.data3 = self.data3[-num_samples:]

            # Update the graphs
            self.update_graphs()

    def read_data(self):
        """Read data from the socket and feed it to the input parser."""
        while True:
            data = s.recv(1024)
            if data:
                self.parse_input(data.decode('utf-8'))
                self.root.update_idletasks()
                self.root.update()

def run_gui():
    root = tk.Tk()
    root.title("Real-Time Line Graphs Visualizer")
    visualizer = LineGraphVisualizer(root)
    root.mainloop()

if __name__ == "__main__":
    run_gui()
