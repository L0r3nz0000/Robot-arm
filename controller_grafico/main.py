from tkinter import ttk
import tkinter as tk
import serial
import pygame
import time

pygame.init()
pygame.joystick.init()

# Inizializza il primo joystick disponibile
if pygame.joystick.get_count() == 0:
  print("Nessun joystick trovato.")
  exit()

joystick = pygame.joystick.Joystick(0)
joystick.init()

print(f"Joystick connesso: {joystick.get_name()}")
print(f"Numero di assi disponibili: {joystick.get_numaxes()}")

PORT = '/dev/ttyACM0' # porta seriale della scheda
BAUDRATE = 115200 # velocità di comunicazione

try:
  ser = serial.Serial(PORT, BAUDRATE)
except:
  print("Impossibile connettersi al robot")
  
x = 0.0
y = 0.0
z = 0.0

try:
  while True:
    pygame.event.pump()  # Necessario per aggiornare gli input

    left_axis_rotation = round(-joystick.get_axis(0), 1) * 4
    left_axis = round(-joystick.get_axis(1), 1) * 4
    right_axis = round(-joystick.get_axis(4), 1) * 4
    
    if left_axis != 0 or right_axis != 0 or left_axis_rotation != 0:
      x += left_axis
      x = min(max(x, 0), 135)
      y += right_axis
      y = min(max(y, 0), x)
      z += left_axis_rotation
      ser.write(f"{int(x+y+z)} {x:.2f} {y:.2f} {z:.2f}\n".encode())
      print(f"Invio: {int(x+y+z)}\t{x:.2f}\t{y:.2f}\t{z:.2f}")
    
    time.sleep(0.1)
except KeyboardInterrupt:
  print("Uscita dallo script.")