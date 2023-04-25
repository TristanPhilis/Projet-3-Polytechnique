from os import setreuid
from flask import Flask, request
from flask_cors import CORS, cross_origin
from flask_restful import Resource, Api
from flask_jsonpify import jsonify
import socketserver
import socket
import struct
from communication_drone import AppchannelTest, Information, communication, Request_type, Command, Command_del

import pymongo
from pymongo import MongoClient

# client = pymongo.MongoClient("mongodb+srv://team101:1234@cluster0.kgtlc.mongodb.net/myFirstDatabase?retryWrites=true&w=majority")
# db = client.Drone
# collection = db.Map
# collection.insert_one({})
# post = {"_id":0, "name": "tim", "score": 5}
# collection.insert_one(post)

app = Flask(__name__)
api = Api(app)

CORS(app)

import logging
import time
from threading import Thread

import struct

import cflib
from cflib.crazyflie import Crazyflie

logging.basicConfig(level=logging.ERROR)

######################################################################################
################################## EMBEDDED SECTION ##################################
######################################################################################
# For now, we hardcode the address
address = [0xE101CF0001, 0xE101CF0002]
# list as Drone dictionnaries
drone_list_embedded = []

@app.route("/connection/drone/unconnect")
def unconnect_all_drone():
    exit_message = "Disconnected with drones: "
    for drone in drone_list_embedded:
        exit_message = exit_message + str(drone['address'])
        drone['drone_link']._close_link()
    drone_list_embedded.clear()
    return jsonify({"Disconnected with drones: ": 'ID'})

@app.route("/connection/drone")
def connection_drone():
    DEBUG_ACTIVATED = True
    drone_list_embedded.clear()
    # Initialize the low-level drivers (don't list the debug drivers)
    cflib.crtp.init_drivers(enable_debug_driver=False)
    # Scan for Crazyflies and use the first one found
    print('Scanning interfaces for Crazyflies...')
    # We look for the addresses and make connections to drones.
    for adr in address:
        available = cflib.crtp.scan_interfaces(adr)
        if DEBUG_ACTIVATED:
            print('available:', available)
        if len(available) > 0 and available[0][0] != 'usb://0':
            # Radio address in format for crazyflies
            radio = available[0][0]
            # Making the connexion to the crazyflie
            drone_link = AppchannelTest(radio)
            drone = {'address': adr, 'radio': radio, 'drone_link': drone_link}
            drone_list_embedded.append(drone)
            available.clear()
    
    if DEBUG_ACTIVATED:
        print('available crazyflies adresses', drone_list_embedded)
    return jsonify({'text': 'ID'})
# End function connection()

def find_drone(address):
    for drone in drone_list_embedded:
        if drone['address'] == address:
            return drone['drone_link'], True
# End function fint_radio_address()

@app.route("/identifydrone1")
def identifydrone1():
    drone_link, found = find_drone(0xE101CF0001)
    if found:
        drone_link._test_appchannel(Request_type.DEL, Command_del.IDENTIFICATION)
    
    return jsonify({'text': 'ID'})
# End function identifydrone1()

@app.route("/identifydrone2")
def identifydrone2():
    drone_link, found = find_drone(0xE101CF0002)
    if found:
        drone_link._test_appchannel(Request_type.DEL, Command_del.IDENTIFICATION)
    
    return jsonify({'text': 'ID'})
# End function identifydrone2()

@app.route("/exploration/drone") #inspired by https://github.com/johnathanchiu/cpp-python-sockets
def takeoff_drone1():
    drone_link, found = find_drone(0xE101CF0001)
    if found:
        drone_link._test_appchannel(Request_type.COMMAND, Command.TAKE_OFF)
    
    return jsonify({'text': 'ID'})
# End function takeoff_drone1()

@app.route("/landing/drone") #inspired by https://github.com/johnathanchiu/cpp-python-sockets
def landing_drone1():
    drone_link, found = find_drone(0xE101CF0001)
    if found:
        drone_link._test_appchannel(Request_type.COMMAND, Command.LANDING)
    
    return jsonify({'text': 'ID'})
# End function takeoff_drone1()

@app.route("/information/drone") #inspired by https://github.com/johnathanchiu/cpp-python-sockets
def information():
    drone_link, found = find_drone(0xE101CF0001)
    if found:
        drone_link._test_appchannel(Request_type.INFORMATION, Information.DRONE_STATE)
    
    return jsonify({'text': 'ID'})
# End function takeoff_drone1()

######################################################################################
################################ END EMBEDDED SECTION ################################
######################################################################################
#                                         ||
#                                         ||
#                                         ||
#                                         ||
#                                         ||
######################################################################################
################################# SIMULATION SECTION #################################
######################################################################################
simConn = []
nbDrone = 2
isSimulation = True
def connection(): #inspired by https://github.com/johnathanchiu/cpp-python-sockets
    try:
        PORT, HOST_IP = 8080, '127.0.0.1'
        for i in range (nbDrone):
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            port = i + PORT 
            sock.bind((HOST_IP, port))
            sock.listen()
            conn, addr = sock.accept()
            simConn.append(conn)
    
    except socket.error as e:
        app.logger.error("error connecting socket to argos " + str(e))
        sock.close()
        return str(e)

@app.route("/takeoff/simulation") #inspired by https://github.com/johnathanchiu/cpp-python-sockets
def takeoff():
    if isSimulation:
        if not simConn:
            connection()
        
        for i in range (nbDrone):
            conn = simConn[i]
            command = 1
            data = struct.pack('I', command)
            conn.sendall(data)
    else:
        return jsonify({'text': 'TO'})


@app.route("/landing/simulation") #inspired by https://github.com/johnathanchiu/cpp-python-sockets
def landing():
    if isSimulation:
        for i in range (nbDrone):
            conn = simConn[i]
            command = 2
            data = struct.pack('I', command)
            conn.sendall(data)
    else:
        return jsonify({'text': 'LDG'}) 
######################################################################################
################################ END EMBEDDED SECTION ################################
######################################################################################


# api.add_resource(Employees, '/employees')  # Route_1
# api.add_resource(EmployeesName, '/employees/<employee_id>')  # Route_3


if __name__ == '__main__':
    app.run(port=5000)


