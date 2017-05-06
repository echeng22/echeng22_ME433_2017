import serial
import matplotlib.pyplot as plt

ser = serial.Serial('/dev/ttyACM0',9600)
ser.write('r')
raw = []
maf = []
iir = []
fir = []

for i in range(100):
    serial_line = ser.readline()
    # print serial_line
    vals = serial_line.split()
    raw.append(vals[1])
    maf.append(vals[2])
    iir.append(vals[3])
    fir.append(vals[4])

ser.close()

plt.plot(raw, label='raw')
plt.plot(maf, label='maf')
plt.plot(iir, label='iir')
plt.plot(fir, label='fir')
plt.legend()
plt.show()

