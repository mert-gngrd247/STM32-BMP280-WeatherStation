import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import collections

# --- CONFIGURATION ---
SERIAL_PORT = 'COM9'  # <--- CHECK THIS
BAUD_RATE = 115200

# --- DATA STORAGE ---
MAX_POINTS = 50
data_temp = collections.deque([0] * MAX_POINTS, maxlen=MAX_POINTS)
data_press = collections.deque([0] * MAX_POINTS, maxlen=MAX_POINTS)

# --- SERIAL CONNECTION ---
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to {SERIAL_PORT}")
except:
    print(f"ERROR: Could not open {SERIAL_PORT}")
    exit()

# --- PLOT SETUP (2 Subplots) ---
fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True, figsize=(8, 6))
fig.suptitle('Weather Station: Temp & Pressure')

def update(frame):
    try:
        if ser.in_waiting:
            line = ser.readline().decode('utf-8').strip()
            parts = line.split(',')
            
            # NOW EXPECTING ONLY 2 VALUES
            if len(parts) == 2:
                t = float(parts[0])
                p = float(parts[1])
                
                # Store
                data_temp.append(t)
                data_press.append(p)
                
                # --- PLOT 1: TEMPERATURE ---
                ax1.cla()
                ax1.set_ylabel('Temp (°C)')
                ax1.plot(data_temp, color='tab:red', label='Temp')
                ax1.grid(True)
                
                # --- PLOT 2: PRESSURE ---
                ax2.cla()
                ax2.set_ylabel('Pressure (hPa)')
                ax2.plot(data_press, color='tab:blue', label='Pressure')
                ax2.grid(True)

    except ValueError:
        pass

# --- RUN ---
ani = FuncAnimation(fig, update, interval=100)
plt.show()
ser.close()