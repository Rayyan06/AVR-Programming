"""
Simple plot for Serial Data
"""

import serial
import matplotlib.pyplot as plt
import sys
import numpy as np

if len(sys.argv) == 3:
    port = sys.argv[1]
    baudrate = int(sys.argv[2])
elif len(sys.argv) == 2:
    print("Insufficient arguments error: port & baudrate should either be provided, or if not provided, will be reset to defaults")
else:
    print("Optional arguments: port, baudrate set to defaults.")
    port = 'COM6'
    baudrate = 9600


# Open serial port
ser = serial.Serial('COM6', 9600)  # Replace 'COMX' with your serial port name

# Flush the serial to remove unwanted data
ser.flush()

# Initialize plot
plt.ion()  # Turn on interactive mode

# Create a new subplot and assign x_data, y_data to it
fig, ax = plt.subplots()
buffer_size = 100  # Number of data points to show at once
x_data, y_data = np.zeros(buffer_size), np.zeros(buffer_size)
line, = ax.plot(x_data, y_data)

ax.set_autoscale_on(True) # Set autoscale on
ax.set_xlim(0, buffer_size)  # Set initial x-axis limit
ax.set_ylim(0, 255)  # Set y-axis limit

plt.xlabel('Time')
plt.ylabel('Value')

plt.title("Simple Oscilloscope")

i = 0
# Main loop to read and plot data
try:
    while True:
        byte = ser.read(1)        # Read byte from serial port

        data_point = ord(byte)        # Convert byte to integer (assuming ASCII encoding)

        # Update data lists
        x_data[i] = i
        y_data[i] = data_point

        # Update plot
        line.set_data(x_data, y_data)

        plt.pause(0.001)

        i = (i + 1) % buffer_size

        # Check for termination
        if plt.waitforbuttonpress(timeout=0.001):
            break

except KeyboardInterrupt:
    print("Plotting stopped by user.")

# Close serial port
ser.close()