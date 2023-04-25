import socket
import struct

from cflib.utils.callbacks import Caller
from threading import Event, Thread



class Drone_information:
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


class SimDrone:
    def __init__(self, socket, port):
        # self.setblocking(False)
        self._socket = socket
        self._port = port

        self._reception_task = Thread(target=self.do_reception)
        self._stop_requested = Event()

        self.connected = Caller()
        self.disconnected = Caller()

        self._reception_task.start()

    def do_reception(self):
        buffer = self._socket.recv(1024)
        view = buffer.decode('utf-8')
        print(f"Received from SimDrone {self._port}: {view}")
        return view

    def send_packet(self, command):
        data = struct.pack("I", command)
        self._socket.send(data)
        print(data)

    def parse_droneInfo(self, data):
        from datetime import datetime, time
        nw = datetime.now()
        info = data.split(",")
        timestamp = str(time(hour=nw.hour, minute=nw.minute, second=nw.second, microsecond=0))
        drone_information = {
            "address" : info[0],
            "drone_position_x" : info[1],
            "drone_position_y" : info[2],
            "drone_battery_level" : info[3],
            "drone_state" : info[4],
            "sensor_front" : info[5],
            "sensor_back" : info[7],
            "sensor_right" : info[8],
            "sensor_left" : info[6],
            "z_angle" : info[9],
            "timestamp" : timestamp}
        return drone_information
