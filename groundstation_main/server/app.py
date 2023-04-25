import argparse
import logging
from PhysicalDroneManager import PhysicalDroneManager
from SimDroneManager import SimDroneManager
from flask import Flask
from flask_cors import CORS
from flask_jsonpify import jsonify
from flask_restful import Api
from flask import send_file
from datetime import datetime, date
import os

################ Logger ################

def getCurrentDate():
    today = date.today()
    current_date = today.strftime("%b-%d-%Y")
    return current_date

def getCurrentTime():
    now = datetime.now()
    current_time = now.strftime("%H:%M:%S")
    return current_time

################################################

# Initializing Flask server
app = Flask(__name__)
api = Api(app)
CORS(app)

parser = argparse.ArgumentParser()
parser.add_argument("--sim", action="store_true")
args = parser.parse_args()

# Initialize drone manager
# TODO load physical manager or simulated manager as needed (env variable)
manager = PhysicalDroneManager() if not args.sim else SimDroneManager()

# Define app routes
@app.route("/identify")
def identify():
    for drone_addr in manager.WHITELIST:
        manager.identify(drone_addr)
    return jsonify({'identify': f'{drone_addr}'})

@app.route("/identifyonedrone/<drone_addr>")
def identify_one_drone(drone_addr):
    manager.identify(int(drone_addr))
    return jsonify({'identify': f'{drone_addr}'})

@app.route("/takeoff")
def takeoff():
    print("calling manager takeoff")
    manager.takeoff()
    print("I am taking off")
    return jsonify({'takeoff': f'takeoff'})

@app.route("/explore")
def explore():
    # Initializing logger
    logName = getCurrentDate()
    logPath = os.path.join("..", "logs")
    if not os.path.isdir(logPath):
        os.mkdir(logPath)
    global currentLogPath
    currentLogPath = '../logs/'+logName+'_'+getCurrentTime()+'.txt'
    logging.warning("Initializing Flask app")
    currentLog = open(currentLogPath, 'w')
    currentLog.write("[ " + getCurrentTime() + " ]: " + "Debut des logs \n")
    currentLog.write("[ " + getCurrentTime() + " ]: " + "Appel a la commande 'Commencer Mission' \n")
    currentLog.close()
    manager.explore()
    return jsonify({'explore': 'explore'})

@app.route("/land")
def land():
    global currentLogPath
    if currentLogPath != "":
        currentLog = open(currentLogPath, 'a')
        currentLog.write("[ " + getCurrentTime() + " ]: " + "Appel a la commande 'Terminer Mission' \n")
        currentLog.close()
        currentLogPath = ""
    manager.land()
    return jsonify({'land': 'land'})

@app.route("/scan")
def scan():
    print("Got scan command")
    if currentLogPath != "":
        currentLog = open(currentLogPath, 'a')
        currentLog.write("[ " + getCurrentTime() + " ]: " + "Appel a la commande 'Scan' \n")
        currentLog.close()
    manager.scan()
    return jsonify({'scan': 'scan'})

@app.route("/dronesupdate")
def drones_update():
    drones_information = manager.drones_information()
    if currentLogPath != "":
        currentLog = open(currentLogPath, 'a')
        currentLog.write(str(drones_information))
        currentLog.write("\n")
        currentLog.close()
    if False:
        print('\nmanager drone list:', drones_information, "\n")
    return jsonify(drones_information)

@app.route("/fetchLogNames")
def fetch_log_names():
    log_names = os.listdir('../logs')
    return jsonify(log_names)

@app.route("/sendfile/<file_name>")
def send_file_to_client(file_name):
    file_path = '../logs/' + file_name
    return send_file(file_path, as_attachment=False)
    
@app.route("/returntobase")
def return_home():
    if currentLogPath != "" :
        currentLog = open(currentLogPath, 'a')
        currentLog.write("[ " + getCurrentTime() + " ]: " + "Appel a la commande 'Retour a la base' \n")
        currentLog.close()
    manager.return_home()
    return jsonify({'returntobase': 'returntobase'})

if __name__ == "__main__":
    # Start drone manager and Flask server
    app.run(host="0.0.0.0")
