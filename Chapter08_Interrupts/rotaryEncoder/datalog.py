import serial
import csv
import time


# Name of output file
output_file = "data.csv"

#  serial settings
port = 'COM6'
baudrate = 9600

ser = serial.Serial(port, baudrate)

# Number of seconds to read
reading_time = 5



time.sleep(2)
with open(output_file, 'w', newline='') as csvfile:
    # Create a csv writer object
    writer = csv.writer(csvfile)

    # Start the timer
    start_time = time.time()

    while(True):
        line = ser.read_until(b'\n').decode().strip()

        print(line)
        if line == '\x03':
            print("End of transmission detected")
            break
        else:
            parts = line.split('\r')
            position = parts[1]
            delta_t = parts[0]
            elapsed_t = int((time.time() - start_time) * 1000)
            writer.writerow([position, delta_t, elapsed_t])

# Close the serial port
ser.close()