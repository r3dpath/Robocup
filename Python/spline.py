import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import CubicSpline, make_interp_spline
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8880  # The port used by the server
s.connect((HOST, PORT))

# Initialize the plot
plt.ion()  # Turn on interactive mode
fig, ax = plt.subplots()
lines = []

# Function to plot the cubic spline
def plot_spline(x, y1, y2, label):
    # Fit a cubic spline through the data points
    #cubic_spline1 = CubicSpline(x, y1)
    #cubic_spline2 = CubicSpline(x, y2)
    for i in range(len(y1)):
        if (int(y1[i]) < int(y2[i])*1.10) and (int(y1[i]) > int(y2[i])*0.90):
            y1[i] = y2[i]

    cubic_spline1 = make_interp_spline(x, y1, k=3)
    cubic_spline2 = make_interp_spline(x, y2, k=3)
    
    # Generate a finer grid for smooth plotting
    x_fine = np.linspace(min(x), max(x), 500)
    y_fine1 = cubic_spline1(x_fine)
    y_fine2 = cubic_spline2(x_fine)
    y_fine = y_fine1 - y_fine2
    
    # Plot the cubic spline
    line, = ax.plot(x_fine, y_fine, 'r', label=label)
    return line

def plot_heading(x, y1, y2, label):
    y1 = [int(i) for i in y1]
    y2 = [int(i) for i in y2]
    for i in range(len(y1)):
        if (y1[i] < y2[i]*1.10 and y1[i] > y2[i]*0.90):
            y1[i] = y2[i]
    
    diff = [y1[i] - y2[i] for i in range(len(y1))]

    if max(diff) > 0:
        angle = x[diff.index(max(diff))]
    else:
        return None

    line, = ax.plot(angle, max(diff), 'bo', label=label)
    return line
    
    
# Initialize the x-coordinates (angles in degrees)
x_coords = [-15, -7.5, 0, 7.5, 15]

# Function to update the plot with new data
def update_plot(sensor1_data, sensor2_data, heading, distance):
    global lines
    
    # Clear previous lines if they exist
    for line in lines:
        line.remove()
    lines.clear()
    
    heading = -(heading-2)*15

    # Plot the new data for each sensor
    line1 = plot_spline(x_coords, sensor1_data, sensor2_data, label='Sensor 1')
    #line2 = plot_heading(x_coords, sensor1_data, sensor2_data, label='Heading')
    line2, = ax.plot(heading, distance, 'bo', label='Heading')
    lines.extend([line1, line2])
    
    # Update plot properties
    ax.set_xlabel('Angle (degrees)')
    ax.set_ylabel('Sensor Value')
    ax.set_title('Cubic Spline Interpolation of Sensor Data')
    ax.legend()
    plt.grid(True)
    plt.draw()
    plt.pause(0.1)

def parse_input(data):
    try:
        data = data.split(':')
        if len(data) != 10:
            return
        sensor1_data, sensor2_data = data[:5], data[5:10]
        heading, distance = int(data[10]), int(data[11])
        print(sensor1_data, sensor2_data, heading, distance)
        update_plot(sensor1_data, sensor2_data, heading, distance)
    except:
        pass

def read_socket():
    while True:
        data = s.recv(1024)
        if data:
            #print(data)
            parse_input(data.decode('utf-8'))

# Example usage
if __name__ == "__main__":
    read_socket()

