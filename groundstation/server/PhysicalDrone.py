import struct
import cflib
from cflib import crazyflie, crtp
from cflib.utils.callbacks import Caller
from cflib.crazyflie.log import LogConfig
from droneState import DroneState

TIME_FOR_EXPLORATION_WHEN_BATTERY_IS_FULL = 360.0 # in 6 minutes, so 360 seconds.
FULL_CHARGE = 100.0
estimatedTimeForMission = 10000.0
battery_drop_per_800_milisecond = 0.0
battery_for_this_mission = 0.0

class PhysicalDrone:
    def __init__(self, address):

        self._crazyflie = crazyflie.Crazyflie()
        self.address = address

        self.connected = Caller()
        self.connection_failed = Caller()
        self.connection_lost = Caller()
        self.disconnected = Caller()

        self._crazyflie.connected.add_callback(self._connected)
        self._crazyflie.connection_failed.add_callback(self._connection_failed)
        self._crazyflie.connection_lost.add_callback(self._connection_lost)
        self._crazyflie.disconnected.add_callback(self._disconnected)
        self._crazyflie.appchannel.packet_received.add_callback(self._packet_received)
        self._crazyflie.console.receivedChar.add_callback(self._debug_print)

    def connect(self, uri):
        self._crazyflie.open_link(uri)
        print("INITIATED CONNECTION")

    def _connected(self, uri):
        print("CONNECTED!!!!!")
        self.connected.call(uri)

        self._lg_stab = LogConfig(name='Drone_information', period_in_ms=800)
        self._lg_stab.add_variable('kalman.stateX', 'float')
        self._lg_stab.add_variable('kalman.stateY', 'float')
        self._lg_stab.add_variable('pm.vbat', 'FP16')
        self._lg_stab.add_variable('information.state', 'int8_t')
        self._lg_stab.add_variable('stateEstimate.yaw', 'float')
        self._lg_dist = LogConfig(name='Ranger', period_in_ms=800)
        self._lg_dist.add_variable('range.front', 'float')
        self._lg_dist.add_variable('range.back', 'float')
        self._lg_dist.add_variable('range.right', 'float')
        self._lg_dist.add_variable('range.left', 'float')

        # Adding the configuration cannot be done until a Crazyflie is
        # connected, since we need to check that the variables we
        # would like to log are in the TOC.
        try:
            self._crazyflie.log.add_config(self._lg_stab)
            # This callback will receive the data
            self._lg_stab.data_received_cb.add_callback(self._stab_log_data)
            # This callback will be called on errors
            self._lg_stab.error_cb.add_callback(self._stab_log_error)
            # Start the logging
            self._lg_stab.start()
        except KeyError as e:
            print('Could not start log configuration,'
                  '{} not found in TOC'.format(str(e)))
        except AttributeError:
            print('Could not add Stabilizer log config, bad configuration.')

        try:
            self._crazyflie.log.add_config(self._lg_dist)
            # This callback will receive the data
            self._lg_dist.data_received_cb.add_callback(self._stab_log_data)
            # This callback will be called on errors
            self._lg_dist.error_cb.add_callback(self._stab_log_error)
            # Start the logging
            self._lg_dist.start()
        except KeyError as e:
            print('Could not start log configuration,'
                  '{} not found in TOC'.format(str(e)))
        except AttributeError:
            print('Could not add Drone_information log config, bad configuration.')

    def _debug_print(self, data):
        print("\n" + data + "\n")

    def _stab_log_error(self, logconf, msg):
        """Callback from the log API when an error occurs"""
        print('Error when logging %s: %s' % (logconf.name, msg))

    def _stab_log_data(self, timestamp, data, logconf):
        """Callback from a the log API when data arrives"""
        # pass
        DEBUG_PRINT = False
        if logconf.name == 'Drone_information':
            self._drone_position_x = 1000 * data['kalman.stateX'] # meters to mm (times 1000)
            self._drone_position_y = 1000 * data['kalman.stateY'] # meters to mm (times 1000)
            self._z_angle = data['stateEstimate.yaw']
            self._drone_state = data['information.state']
            self._drone_battery_level = self.battery_level(self._drone_state, data['pm.vbat'])
            if DEBUG_PRINT:
                print('\n')
                print(data)
                print('drone position x:', self._drone_position_x, 'mm')
                print('drone position y:', self._drone_position_y, 'mm')
                print('drone battery level y:', self._drone_battery_level)
                print('drone position y:', self._drone_state)

        elif logconf.name == 'Ranger':
            self._sensor_front = data['range.front']
            self._sensor_back = data['range.back']
            self._sensor_right = data['range.right']
            self._sensor_left = data['range.left']
            if DEBUG_PRINT:
                print('\n')
                print(data)
                print('sensor_front :', self._sensor_front, 'mm')
                print('sensor_back :', self._sensor_back, 'mm')
                print('sensor_right :', self._sensor_right, 'mm')
                print('sensor_left :', self._sensor_left, 'mm')

    def _connection_failed(self, uri, msg):
        self.connection_failed.call(uri, msg)

    def _connection_lost(self, uri, msg):
        self.connection_lost.call(uri, msg)

    def _disconnected(self, uri):
        self.disconnected.call(uri)

    def _packet_received(self, data):
        response = struct.unpack("<i", data)
        print(f"Drone {self.address:x} received data: {response}")

    def send_packet(self, request_type, command):
        data = struct.pack("<ii", request_type, command)
        self._crazyflie.appchannel.send_packet(data)
        print(f"Sent data: {data} to drone {self.address:x}")
    
    def battery_level(self, drone_state, voltage):
        # Getting the voltage from the crazyfirmware API, from pm.h fonction
        battery = 0

        # If we are in exploration, the battery level is estimated with a time according to its
        # battery level just before the drone's trust
        if(drone_state == DroneState.STATE_EXPLORATION.value or drone_state == DroneState.STATE_RETURN_HOME.value):
            global battery_for_this_mission
            battery_for_this_mission = battery_for_this_mission - battery_drop_per_800_milisecond
            if (battery_for_this_mission < 0.0):
                battery_for_this_mission = 0.0
            battery = int(battery_for_this_mission) 
            return battery

        # Getting the charge % from the voltage table mentionned above.
        if voltage <= 4.2 and voltage > 4.15: battery = 100
        elif voltage <= 4.15 and voltage > 4.11: battery = 95
        elif voltage <= 4.11 and voltage > 4.08: battery = 90
        elif voltage <= 4.08 and voltage > 4.02: battery = 85
        elif voltage <= 4.02 and voltage > 3.98: battery = 80

        elif voltage <= 3.98 and voltage > 3.95: battery = 75
        elif voltage <= 3.95 and voltage > 3.91: battery = 70
        elif voltage <= 3.91 and voltage > 3.87: battery = 65
        elif voltage <= 3.87 and voltage > 3.85: battery = 60

        elif voltage <= 3.85 and voltage > 3.84: battery = 55
        elif voltage <= 3.84 and voltage > 3.82: battery = 50
        elif voltage <= 3.82 and voltage > 3.80: battery = 45
        elif voltage <= 3.80 and voltage > 3.79: battery = 40
        
        elif voltage <= 3.79 and voltage > 3.77: battery = 35
        elif voltage <= 3.77 and voltage > 3.75: battery = 30
        elif voltage <= 3.75 and voltage > 3.73: battery = 25
        elif voltage <= 3.73 and voltage > 3.71: battery = 20
    
        elif voltage <= 3.71 and voltage > 3.69: battery = 15
        elif voltage <= 3.69 and voltage > 3.61: battery = 10
        elif voltage <= 3.61 and voltage > 3.27: battery = 5
        elif voltage <= 3.27: battery = 0
        
        return battery

    def setBatteryLevelEstimateForInFlight(self):
        if hasattr(self, '_drone_position_x'):
            # We did not have a choice but to make these variables global.
            global battery_drop_per_800_milisecond
            global battery_for_this_mission
            # Fetching battery level before launch
            battery_for_this_mission = self._drone_battery_level
            # We estimate how long it can fly
            estimatedTimeFlightForCompleteFlight = (TIME_FOR_EXPLORATION_WHEN_BATTERY_IS_FULL * battery_for_this_mission) / FULL_CHARGE
            # Then we calculate the batery drop per second
            battery_drop_per_second = battery_for_this_mission / estimatedTimeFlightForCompleteFlight
            # Then we calculate the batery drop per 800 milisecond, since it is the LogConfig 
            # module frequency check of log variables.
            battery_drop_per_800_milisecond = 0.8 * battery_drop_per_second
