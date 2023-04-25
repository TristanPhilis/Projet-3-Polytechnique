import { EventEmitter } from '@angular/core';
import { TestBed } from '@angular/core/testing';
import { Drone } from '../interfaces/drone';

import { DroneService } from './drone.service';

describe('DroneService', () => {
  let service: DroneService;
  const messageEmitter: EventEmitter<string> = new EventEmitter<string>();

  // A fake drone.
  const drone = {address: 123,
    drone_position_x: 1234,
    drone_position_y: 12345,
    drone_battery_level: 25,
    drone_state: 1234567,
    sensor_front: 10,
    sensor_back: 1150,
    sensor_right: 200,
    sensor_left: 315,
    z_angle: 340,
    timestamp: "2021-12-8"
  } as Drone;

  // Another fake drone.
  const drone2 = {address: 12345,
    drone_position_x: 1234,
    drone_position_y: 12345,
    drone_battery_level: 25,
    drone_state: 1234567,
    sensor_front: 10,
    sensor_back: 1150,
    sensor_right: 200,
    sensor_left: 315,
    z_angle: 340,
    timestamp: "2021-12-8"
  } as Drone;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(DroneService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });

  it('should update drones list', () => {
    service['messageEmitter'] = messageEmitter;
    spyOn(service, 'emitNewDronesInfo');
    
    // Empty list
    let droneList:Drone[] = []
    service.drones = [];
    service.dronesUpdate(droneList);
    expect(service['emitNewDronesInfo']).toHaveBeenCalled();

    // list not empty with drone not already inside
    droneList.push(drone);
    service.dronesUpdate(droneList);
    expect(service['emitNewDronesInfo']).toHaveBeenCalled();

    // list not empty with drone already inside
    service.dronesUpdate(droneList);
    expect(service['emitNewDronesInfo']).toHaveBeenCalled();
  });

  it('should return the drone index by address', () => {
    // Test with empty list
    let droneList:Drone[] = []
    let index = service.getDroneIndex(drone.address);
    expect(index).toEqual(-1);    

    // Test with one drone in the list
    droneList.push(drone);
    service.drones = [];
    service.dronesUpdate(droneList);
    index = service.getDroneIndex(drone.address);
    expect(index).toEqual(0);    

    // Test with one drone in the list with a second drone
    droneList.push(drone2);
    service.drones = droneList;
    index = service.getDroneIndex(drone2.address);
    const Drone2_index = 1;
    expect(index).toEqual(Drone2_index);    
  });

  it('should return the drone s battery level and state, as well as the timestamp', () => {
    let droneList:Drone[] = []
    droneList.push(drone);
    // Let's put one drone in the list
    service.drones = droneList;
    // The index should be 0 logically, because we only have one drone
    const index = 0;

    // Testing for battery level
    const batteryLevel = service.getBatteryLevel(index);
    expect(batteryLevel).toEqual(drone.drone_battery_level);

    // Testing for battery level
    const droneState = service.getDroneState(index);
    expect(droneState).toEqual(drone.drone_state);

    // Testing for battery level
    const timeStamp = service.getTimeStamp(index);
    expect(timeStamp).toEqual(drone.timestamp);
  });

  it('should return neither the drone s battery level, nor state, not even the timestamp', () => {
    let droneList:Drone[] = []
    droneList.push(drone);
    // Let's put one drone in the list
    service.drones = droneList;
    // The index should be 0 logically, because we only have one drone
    const index = 3;

    // Testing for battery level
    const batteryLevel = service.getBatteryLevel(index);
    expect(batteryLevel).not.toEqual(drone.drone_battery_level);

    // Testing for battery level
    const droneState = service.getDroneState(index);
    expect(droneState).not.toEqual(drone.drone_state);

    // Testing for battery level
    const timeStamp = service.getTimeStamp(index);
    expect(timeStamp).not.toEqual(drone.timestamp);
  });

  it('should tell us if the drone is already in the list', () => {
    let droneList:Drone[] = []
    let isDroneAlreadyInList;
    
    // Test with the drone not in the list
    droneList.push(drone)
    service.drones = droneList;
    isDroneAlreadyInList = service.droneAlreadyInList(drone2.address);
    expect(isDroneAlreadyInList).toBeFalse();
    
    // Test with the drone in the list
    droneList.push(drone2);
    service.drones = droneList;
    isDroneAlreadyInList = service.droneAlreadyInList(drone.address);
    expect(isDroneAlreadyInList).toBeTrue();
  });

  it('should emit with message updateInformation', () => {
    // let droneList:Drone[] = []
    service['messageEmitter'] = messageEmitter;
    spyOn(service.messageEmitter, 'emit'); 
    service.emitNewDronesInfo();
    expect(service.messageEmitter.emit).toHaveBeenCalledWith('updateInformation');
  });
});
