#     self._lg_stab = LogConfig(name='Drone_information', period_in_ms=500)
    #     self._lg_stab.add_variable('kalman.stateX', 'float')
    #     self._lg_stab.add_variable('kalman.stateY', 'float')
    #     self._lg_stab.add_variable('pm.vbat', 'FP16')
    #     self._lg_stab.add_variable('information.state', 'int8_t')
    #     self._lg_dist = LogConfig(name='Ranger', period_in_ms=500)
    #     self._lg_dist.add_variable('range.front', 'float')
    #     self._lg_dist.add_variable('range.back', 'float')
    #     self._lg_dist.add_variable('range.right', 'float')
    #     self._lg_dist.add_variable('range.left', 'float')

    #     # Adding the configuration cannot be done until a Crazyflie is
    #     # connected, since we need to check that the variables we
    #     # would like to log are in the TOC.
    #     try:
    #         self._crazyflie.log.add_config(self._lg_stab)
    #         # This callback will receive the data
    #         self._lg_stab.data_received_cb.add_callback(self._stab_log_data)
    #         # This callback will be called on errors
    #         self._lg_stab.error_cb.add_callback(self._stab_log_error)
    #         # Start the logging
    #         self._lg_stab.start()
    #     except KeyError as e:
    #         print('Could not start log configuration,'
    #               '{} not found in TOC'.format(str(e)))
    #     except AttributeError:
    #         print('Could not add Stabilizer log config, bad configuration.')

    #     try:
    #         self._crazyflie.log.add_config(self._lg_dist)
    #         # This callback will receive the data
    #         self._lg_dist.data_received_cb.add_callback(self._stab_log_data)
    #         # This callback will be called on errors
    #         self._lg_dist.error_cb.add_callback(self._stab_log_error)
    #         # Start the logging
    #         self._lg_dist.start()
    #     except KeyError as e:
    #         print('Could not start log configuration,'
    #               '{} not found in TOC'.format(str(e)))
    #     except AttributeError:
    #         print('Could not add Drone_information log config, bad configuration.')

    # def _stab_log_error(self, logconf, msg):
    #     """Callback from the log API when an error occurs"""
    #     print('Error when logging %s: %s' % (logconf.name, msg))

    # def _stab_log_data(self, timestamp, data, logconf):
    #     """Callback from a the log API when data arrives"""
    #     # pass
    #     DEBUG_PRINT = True
    #     print('\n')
    #     if logconf.name == 'Drone_information':
    #         print(data)
    #         self._drone_position_x = 1000 * data['kalman.stateX'] # meters to mm (times 1000)
    #         self._drone_position_y = 1000 * data['kalman.stateY'] # meters to mm (times 1000)
    #         self._drone_battery_level = data['pm.vbat']
    #         self._drone_state = data['information.state']
    #         if DEBUG_PRINT:
    #             print('drone position x:', self._drone_position_x, 'mm')
    #             print('drone position y:', self._drone_position_y, 'mm')
    #             print('drone battery level y:', self._drone_battery_level)
    #             print('drone position y:', self._drone_state)

    #     elif logconf.name == 'Ranger':
    #         print(data)
    #         self.sensor_front = data['range.front']
    #         self.sensor_back = data['range.back']
    #         self.sensor_right = data['range.right']
    #         self.sensor_left = data['range.left']
    #         if DEBUG_PRINT:
    #             print('sensor_front :', self.sensor_front, 'mm')
    #             print('sensor_back :', self.sensor_back, 'mm')
    #             print('sensor_right :', self.sensor_right, 'mm')
    #             print('sensor_left :', self.sensor_left, 'mm')