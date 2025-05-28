import serial
from flask import Flask, request, jsonify, render_template
from flask_cors import CORS
import socket

PORT = '/dev/ttyACM0' # porta seriale del robot
BAUDRATE = 115200 # velocità di comunicazione

try:
  ser = serial.Serial(PORT, BAUDRATE)
except:
  print("Impossibile connettersi al robot")
  exit(1)
  
movement = [0, 0, 0] # -1 = down, 0 = stop, 1 = up

app = Flask(__name__)
CORS(app)

def get_private_ip():
  s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  try:
    # Questo non deve andare a buon fine, serve solo per scoprire l'IP della macchina
    s.connect(("8.8.8.8", 80))
    ip = s.getsockname()[0]
  except Exception:
    ip = "127.0.0.1"
  finally:
    s.close()
  return ip

@app.route("/", methods=['GET', 'POST'])
def update_state():
  if request.method == 'GET':
    server_ip = get_private_ip()
    return render_template("index.html", server_ip=server_ip)
  
  elif request.method == 'POST':
    data = request.get_json()
    id = int(data["id"])
    action = data["action"]
    
    if id >= 0 and id <= 5:
      
      if action == "release":
        match id:
          case 0:  # left controller, up button
            movement[0] = 0
          case 1:  # left controller, right button
            movement[2] = 0
          case 2:  # left controller, down button
            movement[0] = 0
          case 3:  # left controller, left button
            movement[2] = 0
          case 4:  # right controller, up button
            movement[1] = 0
          case 5:  # right controller, down button
            movement[1] = 0
        
      elif action == "press":
        
        match id:
          case 0:  # left controller, up button
            movement[0] = 1
          case 1:  # left controller, right button
            movement[2] = -1
          case 2:  # left controller, down button
            movement[0] = -1
          case 3:  # left controller, left button
            movement[2] = 1
          case 4:  # right controller, up button
            movement[1] = 1
          case 5:  # right controller, down button
            movement[1] = -1
      else:
        jsonify({'error': 'unknown action'}), 400
    else:
      return jsonify({'error': 'invalid id'}), 400
    
    ser.write(f"\t{movement[0]} {movement[1]} {movement[2]}\n".encode())
    print("sending:", movement)
    print(ser.readline().decode())
    
    return jsonify({'message': 'ok'}), 200
  
if __name__ == "__main__":
  app.run(host="0.0.0.0", port=3030)