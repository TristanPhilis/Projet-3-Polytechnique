from cflib import crtp
from enum import Enum
from PhysicalDrone import PhysicalDrone
from AbstractDroneManager import AbstractDroneManager


def uri_to_address(uri: str) -> int:
    return int(uri.split('/')[-1], base=16)


class Command(Enum):
    TAKE_OFF = 0
    LAND = 1
    RETURN_TO_BASE = 2
    EXPLORE = 3


class CommandLED(Enum):
    IDENTIFICATION = 0
    P2P_DEL_COLOR_CHANGE = 1


class RequestType(Enum):
    LED = 0
    COMMAND = 1
    INFORMATION = 2


class Information(Enum):
    DRONE_STATE = 0
    BATTERY_LEVEL = 1

# class Complex:
# ...     def __init__(self, realpart, imagpart):
# ...         self.r = realpart
# ...         self.i = imagpart

class drone_information:
    def __init__(self, a, px, py, bl, ds, sf, sb, sr, sl, ts):
        self.address = a 
        self.drone_position_x = px
        self.drone_position_y = py
        self.drone_battery_level = bl
        self.drone_state = ds
        self.sensor_front = sf
        self.sensor_back = sb
        self.sensor_right = sr
        self.sensor_left = sl
        self.timestamp = ts



class PhysicalDroneManager(AbstractDroneManager):
    def __init__(self):
        self.WHITELIST = [
            0xE101CF0001,
            0xE101CF0002
        ]

        self._connected_drones = dict()
        crtp.init_drivers(enable_debug_driver=False)

    def scan(self):
        if len(self._connected_drones) == len(self.WHITELIST):
            # All whitelisted drones have been connected, no need to scan
            return

        for address in self.WHITELIST:
            if address not in self._connected_drones.keys():
                print(f"Scanning for {address:x}")
                available = crtp.scan_interfaces(address)

                if len(available) > 0:
                    drone = PhysicalDrone(address)

                    drone.connected.add_callback(self.drone_connected)
                    drone.disconnected.add_callback(self.drone_disconnected)
                    drone.connection_failed.add_callback(self.drone_connection_failed)
                    drone.connection_lost.add_callback(self.drone_connection_lost)

                    drone.connect(available[0][0])
                    self._connected_drones[address] = drone

    def drone_connected(self, uri):
        print(f"Connected to drone {uri_to_address(uri):x}")

    def drone_disconnected(self, uri):
        del self._connected_drones[uri_to_address(uri)]
        print(f"Disconnected from drone {uri_to_address(uri):x}")

    def drone_connection_failed(self, uri, msg):
        print(msg)

    def drone_connection_lost(self, uri, msg):
        print(msg)

    def identify(self, address):
        if address in self._connected_drones.keys():
            drone = self._connected_drones.get(address)
            drone.send_packet(RequestType.LED.value, CommandLED.IDENTIFICATION.value)

    def takeoff(self):
        for drone_addr in self.WHITELIST:
            if drone_addr in self._connected_drones.keys():
                drone = self._connected_drones.get(drone_addr)
                drone.send_packet(RequestType.COMMAND.value, Command.TAKE_OFF.value)

    def land(self):
        for drone_addr in self.WHITELIST:
            if drone_addr in self._connected_drones.keys():
                drone = self._connected_drones.get(drone_addr)
                drone.send_packet(RequestType.COMMAND.value, Command.LAND.value)
    
    def explore(self):
        for drone_addr in self.WHITELIST:
            if drone_addr in self._connected_drones.keys():
                drone = self._connected_drones.get(drone_addr)
                drone.setBatteryLevelEstimateForInFlight()
                drone.send_packet(RequestType.COMMAND.value, Command.EXPLORE.value)

    def drones_information(self):
        from datetime import datetime, time
        nw = datetime.now()
        timestamp = str(time(hour=nw.hour, minute=nw.minute, second=nw.second, microsecond=0))
        drones_information = []
        for key in self._connected_drones:
            drone = self._connected_drones[key]
            if hasattr(drone, '_drone_position_x'):
                drone_information = {
                "address" : key,
                "drone_position_x" : drone._drone_position_x,
                "drone_position_y" : drone._drone_position_y,
                "drone_battery_level" : drone._drone_battery_level,
                "drone_state" : drone._drone_state,
                "sensor_front" : drone._sensor_front,
                "sensor_back" : drone._sensor_back,
                "sensor_right" : drone._sensor_right,
                "sensor_left" : drone._sensor_left,
                "z_angle" : drone._z_angle,
                "timestamp" : timestamp}
                
                # drone_information2 = drone_information(key, drone._drone_position_x, drone._drone_position_y, drone._drone_battery_level, drone._drone_state, drone._sensor_front, drone._sensor_back, drone._sensor_right, drone._sensor_left, timestamp)
                drones_information.append(drone_information)
        return drones_information


    def return_home(self):
        for drone_addr in self.WHITELIST:
            if drone_addr in self._connected_drones.keys():
                drone = self._connected_drones.get(drone_addr)
                drone.send_packet(RequestType.COMMAND.value, Command.RETURN_TO_BASE.value)
