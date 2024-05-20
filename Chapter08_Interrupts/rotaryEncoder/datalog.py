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
reading_time = 10




with open(output_file, 'w', newline='') as csvfile:
    # Create a csv writer object
    writer = csv.writer(csvfile)

    # Start the timer
    start_time = time.time()

    while(time.time() - start_time < reading_time):
        line = ser.read_until(b'\n').decode().rstrip('\n').split('\r')
        position = line[0]
        delta_t = line[1]
        elapsed_t = int((time.time() - start_time) * 1000)
        writer.writerow([position, delta_t, elapsed_t])

# Close the serial port
ser.close()