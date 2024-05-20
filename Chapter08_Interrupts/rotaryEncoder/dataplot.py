import serial
import matplotlib.pyplot as plt

# Define the number of values to read at a time
num_values = 99

# Initialize lists to store positions and times
positions = []
times_ms = []

# Open serial port
ser = serial.Serial('COM6', timeout = 4)  # Change 'COM6' to your serial port

# Plotting Setup
plt.ion() # interactive mode
fig, ax1 = plt.subplots(figsize=(10, 6))


# Create second y-axis for times
ax2 = ax1.twinx()

# Set plot titles and labels
ax1.set_title("Position vs Time")
ax1.set_xlabel("Index")
ax1.set_ylabel("Position")
ax2.set_ylabel("Time (ms)")

# Initialize empty plots for positions and times
position_line, = ax1.plot([], [], color='blue', marker='o', label='Position')
time_line, = ax2.plot([], [], color='green', marker='o', label='Time (ms)')

# Show legend
ax1.legend(loc='upper left')
ax2.legend(loc='upper right')

try:
    # Read data from serial
    input_data = ser.read(num_values * 7)

    if input_data:
            # Split the input data into individual lines
        data_lines = input_data.decode().strip().split('\r\n')

        # Extract positions and times
        for i in range(0, len(data_lines), 2):

            if i + 1 < len(data_lines):  # Check if there are enough lines
                position_str, time_str = data_lines[i], data_lines[i+1]                
                print(position_str)
                print(time_str)
                position = int(position_str)

                time_ticks = int(time_str)
                time_ms = time_ticks / 250

                positions.append(position)
                times_ms.append(time_ms)
        # Update plots
        position_line.set_xdata(range(len(positions)))
        position_line.set_ydata(positions)
        time_line.set_xdata(range(len(times_ms)))
        time_line.set_ydata(times_ms)
        ax1.relim()
        ax1.autoscale_view(True, True, True)
        ax2.relim()
        ax2.autoscale_view(True, True, True)


    plt.draw()
    plt.pause(0.001)  # Pause to allow the plot to update

except KeyboardInterrupt:
    pass  # Allow the loop to exit gracefully on Ctrl+C

finally:
    ser.close()  # Close serial port