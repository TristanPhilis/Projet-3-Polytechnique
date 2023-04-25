# -*- coding: utf-8 -*-
#
#     ||          ____  _ __
#  +------+      / __ )(_) /_______________ _____  ___
#  | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
#  +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
#   ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
#
#  Copyright (C) 2014 Bitcraze AB
#
#  Crazyflie Nano Quadcopter Client
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA  02110-1301, USA.
"""
Simple example that connects to the first Crazyflie found, Sends and
receive appchannel packets

The protocol is:
 - 3 floats are send, x, y and z
 - The Crazyflie sends back the sum as one float
"""
import logging
import time
from threading import Thread

import struct

import cflib
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.log import LogConfig

logging.basicConfig(level=logging.ERROR)

from enum import Enum

class Command:
    TAKE_OFF = 0
    LANDING = 1
    RETURN_TO_BASE = 2
    EXPLORATION = 3

class Command_del:
    IDENTIFICATION = 0
    P2P_DEL_COLOR_CHANGING = 1

class Request_type:
    DEL = 0
    COMMAND = 1
    INFORMATION = 2

class Information:
    DRONE_STATE = 0
    BATTERY_LEVEL = 1
    DRONE_COORDINATES = 2


class AppchannelTest:
    """Example that connects to a Crazyflie and ramps the motors up/down and
    the disconnects"""

    def __init__(self, link_uri):
        """ Initialize and run the example with the specified link_uri """
        self._cf = Crazyflie()

        self._cf.connected.add_callback(self._connected)
        self._cf.disconnected.add_callback(self._disconnected)
        self._cf.connection_failed.add_callback(self._connection_failed)
        self._cf.connection_lost.add_callback(self._connection_lost)

        self._cf.appchannel.packet_received.add_callback(self._app_packet_received)

        self._cf.open_link(link_uri)

        print('Connecting to %s' % link_uri)


    def _connected(self, link_uri):
        """ This callback is called form the Crazyflie API when a Crazyflie
        has been connected and the TOCs have been downloaded."""

        self._lg_stab = LogConfig(name='Drone_information', period_in_ms=500)
        self._lg_stab.add_variable('kalman.stateX', 'float')
        self._lg_stab.add_variable('kalman.stateY', 'float')
        self._lg_stab.add_variable('pm.vbat', 'FP16')
        self._lg_stab.add_variable('information.state', 'int8_t')
        self._lg_dist = LogConfig(name='Ranger', period_in_ms=500)
        self._lg_dist.add_variable('range.front', 'float')
        self._lg_dist.add_variable('range.back', 'float')
        self._lg_dist.add_variable('range.right', 'float')
        self._lg_dist.add_variable('range.left', 'float')

        # Adding the configuration cannot be done until a Crazyflie is
        # connected, since we need to check that the variables we
        # would like to log are in the TOC.
        try:
            self._cf.log.add_config(self._lg_stab)
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
            self._cf.log.add_config(self._lg_dist)
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

        # Start a separate thread to do the motor test.
        # Do not hijack the calling thread!
        # Thread(target=self._test_appchannel).start()

    def _stab_log_error(self, logconf, msg):
        """Callback from the log API when an error occurs"""
        print('Error when logging %s: %s' % (logconf.name, msg))

    def _stab_log_data(self, timestamp, data, logconf):
        """Callback from a the log API when data arrives"""
        # pass
        DEBUG_PRINT = True
        print('\n')
        if logconf.name == 'Drone_information':
            print(data)
            self._drone_position_x = 1000 * data['kalman.stateX'] # meters to mm (times 1000)
            self._drone_position_y = 1000 * data['kalman.stateY'] # meters to mm (times 1000)
            self._drone_battery_level = data['pm.vbat']
            self._drone_state = data['information.state']
            if DEBUG_PRINT:
                print('drone position x:', self._drone_position_x, 'mm')
                print('drone position y:', self._drone_position_y, 'mm')
                print('drone battery level y:', self._drone_battery_level)
                print('drone state y:', self._drone_state)

        elif logconf.name == 'Ranger':
            print(data)
            self.sensor_front = data['range.front']
            self.sensor_back = data['range.back']
            self.sensor_right = data['range.right']
            self.sensor_left = data['range.left']
            if DEBUG_PRINT:
                print('sensor_front :', self.sensor_front, 'mm')
                print('sensor_back :', self.sensor_back, 'mm')
                print('sensor_right :', self.sensor_right, 'mm')
                print('sensor_left :', self.sensor_left, 'mm')

    def _connection_failed(self, link_uri, msg):
        """Callback when connection initial connection fails (i.e no Crazyflie
        at the specified address)"""
        print('Connection to %s failed: %s' % (link_uri, msg))

    def _connection_lost(self, link_uri, msg):
        """Callback when disconnected after a connection has been made (i.e
        Crazyflie moves out of range)"""
        print('Connection to %s lost: %s' % (link_uri, msg))

    def _disconnected(self, link_uri):
        """Callback when the Crazyflie is disconnected (called in all cases)"""
        print('Disconnected from %s' % link_uri)

    def _app_packet_received(self, data):

        response = struct.unpack("if", data)
        print(f"\nReponse: {response}")

    def _test_appchannel(self, request_type, command):
        data = struct.pack("<ii", request_type, command)
        self._cf.appchannel.send_packet(data)

        time.sleep(1)

    def _close_link(self):
        self._cf.close_link()

    # def information(self):
        


def communication(radio_address, request_type, command):
    # le = AppchannelTest(radio_address, request_type, command)
    pass


if __name__ == '__main__':
    # Initialize the low-level drivers (don't list the debug drivers)
    cflib.crtp.init_drivers(enable_debug_driver=False)
    # Scan for Crazyflies and use the first one found
    print('Scanning interfaces for Crazyflies...')
    # For now, we hardcode the address
    address = 0xE101CF0001
    available = cflib.crtp.scan_interfaces(address)
    # available = cflib.crtp.scan_interfaces()
    print('Crazyflies found:')
    for i in available:
        print(i[0])

    if len(available) > 0:
        le = AppchannelTest(available[0][0])
    else:
        print('No Crazyflies found, cannot run example')



# # The following Battery level table is taken from https://blog.ampow.com/lipo-voltage-chart/
# # Table is rearranged because we only need 1S column according to the 3.7V battery

# # ---------------------------------------------------------------
# # |        Table of Voltage and Capacity for 1S for 3.7V        |
# # ---------------------------------------------------------------
# # |           |       |
# # | Capacity  |  1S   |
# # |     %     |       |
# # ---------------------
# # |    100    |  4.2  |
# # |     95    |  4.15 |
# # |     90    |  4.11 |
# # |     85    |  4.08 |
# # |     80    |  4.02 |
# # |     75    |  3.98 |
# # |     70    |  3.95 |
# # |     65    |  3.91 |
# # |     60    |  3.87 |
# # |     55    |  3.85 |
# # |     50    |  3.84 |
# # |     45    |  3.82 |
# # |     40    |  3.80 |
# # |     35    |  3.79 |
# # |     30    |  3.77 |
# # |     25    |  3.75 |
# # |     20    |  3.73 |
# # |     15    |  3.71 |
# # |     10    |  3.69 |
# # |      5    |  3.61 |
# # |      0    |  3.27 |
# # ---------------------

# def prepareBatteryLevel(voltage):
#     # Getting the voltage from the crazyfirmware API, from pm.h fonction
#     battery = 0

#     # Getting the charge % from the voltage table mentionned above.
#     if (voltage <= 4.2 and voltage > 4.15): battery = 100
#     elif (voltage <= 4.15 and voltage > 4.11): battery = 95
#     elif (voltage <= 4.11 and voltage > 4.08): battery = 90
#     elif (voltage <= 4.08 and voltage > 4.02): battery = 85
#     elif (voltage <= 4.02 and voltage > 3.98): battery = 80

#     elif (voltage <= 3.98 and voltage > 3.95): battery = 75
#     elif (voltage <= 3.95 and voltage > 3.91): battery = 70
#     elif (voltage <= 3.91 and voltage > 3.87): battery = 65
#     elif (voltage <= 3.87 and voltage > 3.85): battery = 60

#     elif (voltage <= 3.85 and voltage > 3.84): battery = 55
#     elif (voltage <= 3.84 and voltage > 3.82): battery = 50
#     elif (voltage <= 3.82 and voltage > 3.80): battery = 45
#     elif (voltage <= 3.80 and voltage > 3.79): battery = 40
    
#     elif (voltage <= 3.79 and voltage > 3.77): battery = 35
#     elif (voltage <= 3.77 and voltage > 3.75): battery = 30
#     elif (voltage <= 3.75 and voltage > 3.73): battery = 25
#     elif (voltage <= 3.73 and voltage > 3.71): battery = 20
 
#     elif (voltage <= 3.71 and voltage > 3.69): battery = 15
#     elif (voltage <= 3.69 and voltage > 3.61): battery = 10
#     elif (voltage <= 3.61 and voltage > 3.27): battery = 5
#     elif (voltage <= 3.27): battery = 0
