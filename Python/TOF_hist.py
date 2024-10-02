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


class ReductionVisualizer:
    def __init__(self, root):
        self.root = root
        self.top_reductions = []  # Store top reductions for live histogram

        # Matplotlib figure setup for histogram
        self.fig, self.ax = plt.subplots(figsize=(5, 4))
        self.canvas_plot = FigureCanvasTkAgg(self.fig, master=root)  # Tkinter-compatible canvas for matplotlib
        self.canvas_plot.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)

        self.ax.set_title("Top Average Reductions Histogram")
        self.ax.set_xlabel("Reduction (%)")
        self.ax.set_ylabel("Frequency")

        # Thread to simulate or receive live data
        self.serial_thread = threading.Thread(target=self.read_data)
        self.serial_thread.daemon = True
        self.serial_thread.start()

    def update_histogram(self, new_reduction):
        """Update the histogram with the new top average reduction."""
        self.top_reductions.append(new_reduction)

        # Clear and update the histogram
        self.ax.cla()
        self.ax.hist(self.top_reductions, bins=100, color='blue', edgecolor='black')
        self.ax.set_title("Top Average Reductions Histogram")
        self.ax.set_xlabel("Reduction (%)")
        self.ax.set_ylabel("Frequency")

        self.canvas_plot.draw()

    def calcs(self, distances):
        """Calculate top average reduction and update histogram."""
        distances = [int(distance) for distance in distances]
        distances = [dist if dist < 1800 else 1800 for dist in distances ]
        diffs = np.array(distances[0:5]) - np.array(distances[5:10])
        idx_max = diffs.argmax()
        reduction = (diffs[idx_max] / distances[idx_max])*100
        print(f"Difference as percent of top reading: {reduction:.2f}%")

        # Update the live histogram with the new top average reduction
        self.update_histogram(reduction)

    def parse_input(self, data):
        """Parse received data and trigger calculations."""
        for dp in data.splitlines():
            distances = dp.split(':')
            if distances[0] == 'S':
                distances = distances[1:11]
                self.calcs(distances)

    def read_data(self):
        """Simulate or read data from the socket."""
        while True:
            data = s.recv(1024)
            if data:
                self.parse_input(data.decode('utf-8'))
                self.root.update_idletasks()
                self.root.update()


def run_gui():
    root = tk.Tk()
    root.title("Top Average Reduction Visualizer")
    visualizer = ReductionVisualizer(root)
    root.mainloop()


if __name__ == "__main__":
    run_gui()
