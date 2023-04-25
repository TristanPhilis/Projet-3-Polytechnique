import socket
from enum import Enum
from threading import Event, Thread
from SimDrone import SimDrone
from AbstractDroneManager import AbstractDroneManager

HOST = "0.0.0.0"
PORT = 8080

class Command(Enum):
    EXPLORE = 1
    LAND = 2
    RETURN_TO_BASE = 3
    GET_INFORMATION = 4

class SimDroneManager(AbstractDroneManager):
    def __init__(self):
        self._connected_drones = dict()
        self._server_socket = None

        self._scanning_task = Thread(target=self.scan)
        self._stop_requested = Event()

        self._server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._server_socket.bind((HOST, PORT))
        self._server_socket.listen(5)

        self._scanning_task.start()

    def scan(self):
        while not self._stop_requested.is_set():
            try:
                (drone_socket, drone_address) = self._server_socket.accept()
                drone = SimDrone(drone_socket, drone_address)
                self._connected_drones[drone_address] = drone
                print(f"Connected to simulated drone {drone_address}")
            except socket.error as e:
                print(e)
                drone_socket.close()

    def land(self):
        for port in self._connected_drones:
            sim_drone = self._connected_drones.get(port)
            sim_drone.send_packet(Command.LAND.value)

    def explore(self):
        for port in self._connected_drones:
            sim_drone = self._connected_drones.get(port)
            sim_drone.send_packet(Command.EXPLORE.value)

    def drones_information(self):
        for port in self._connected_drones:
            sim_drone = self._connected_drones.get(port)
            sim_drone.send_packet(Command.GET_INFORMATION.value)
        
        drones_information = []
        for port in self._connected_drones:
            sim_drone = self._connected_drones.get(port)
            data = sim_drone.do_reception()
            drone_information = sim_drone.parse_droneInfo(data)
            drones_information.append(drone_information)
            
        return drones_information
    
    def return_home(self):
        for port in self._connected_drones:
            sim_drone = self._connected_drones.get(port)
            sim_drone.send_packet(Command.RETURN_TO_BASE.value)
