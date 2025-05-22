from tkinter import ttk
import tkinter as tk
import serial

PORT = '/dev/ttyUSB0' # porta seriale della scheda
BAUDRATE = 115200 # velocità di comunicazione

try:
  ser = serial.Serial(PORT, BAUDRATE)
  ser.write("G90\n".encode())  # Posizionamento assoluto
  ser.write("G92 X0 Y0 Z0\n".encode())  # Setta gli zeri
except:
  print("Impossibile connettersi al robot")

def on_value_change(event=None):
  x = x_var.get()
  y = y_var.get()
  z = z_var.get()
  ser.write(f"G1 X{x} Y{y} Z{z}".encode())

root = tk.Tk()
root.title("Slider con Callback")

main_frame = ttk.Frame(root, padding=10)
main_frame.pack(fill='both', expand=True)

# Variabili per gli slider
x_var = tk.IntVar(value=0)
y_var = tk.IntVar(value=0)
z_var = tk.IntVar(value=0)

def create_slider(label_text, variable):
  row = ttk.Frame(main_frame)
  row.pack(fill='x', pady=5)

  label = ttk.Label(row, text=f"{label_text}:", width=3) 
  label.pack(side='left')

  slider = ttk.Scale(row, from_=0, to=360, orient='horizontal',
                      variable=variable, command=on_value_change)
  slider.pack(side='left', fill='x', expand=True)

# Creazione degli slider con etichette
create_slider("X", x_var)
create_slider("Y", y_var)
create_slider("Z", z_var)

root.mainloop()
