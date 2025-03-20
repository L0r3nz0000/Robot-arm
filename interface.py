from pynput.mouse import Controller
import serial
import time

# Crea un oggetto Controller per il mouse
mouse = Controller()
ser = serial.Serial('/dev/ttyACM0', 9600)

# Ottieni la posizione corrente del mouse
while True:
  angle = mouse.position[0] / 1920 * 180
  print(angle)
  ser.write((str(int(angle))+";").encode())
  ser.flush()
  time.sleep(0.05)