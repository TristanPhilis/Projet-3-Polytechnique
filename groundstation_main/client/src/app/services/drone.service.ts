import { Injectable, EventEmitter } from '@angular/core';
import { Drone } from '../interfaces/drone';

@Injectable({
  providedIn: 'root'
})
export class DroneService {
  DroneWhiteList: number[] = [];
  drones: Drone[] = []
  messageEmitter: EventEmitter<string> = new EventEmitter<string>();
  mapEmitter: EventEmitter<string> = new EventEmitter<string>();

  constructor() { }

  dronesUpdate(drones: Drone[]) {
    // this.drones = []
    // this.drones = drones;
    console.log('hello from dronesUpdate()');
    for(let i = 0; i < drones.length; i++){
      if (!this.droneAlreadyInList(drones[i].address))
        this.drones.push(drones[i]);
      else {
        const index = this.getDroneIndex(drones[i].address);
        this.drones[index] = drones[i];
      }
    }

    for(let i = 0; i < this.drones.length; i++){
      console.log("this.drone[", i, "]: ", this.drones[i]);
      console.log("this.drones[", i, "]",".address:", this.drones[i].address);
      console.log("this.drones[", i, "]",".drone_position_x:", this.drones[i].drone_position_x);
      console.log("this.drones[", i, "]",".drone_position_y:", this.drones[i].drone_position_y);
      console.log("this.drones[", i, "]",".drone_battery_level:", this.drones[i].drone_battery_level);
      console.log("this.drones[", i, "]",".drone_state:", this.drones[i].drone_state);
      console.log("this.drones[", i, "]",".sensor_front:", this.drones[i].sensor_front);
      console.log("this.drones[", i, "]",".sensor_back:", this.drones[i].sensor_back);
      console.log("this.drones[", i, "]",".sensor_right:", this.drones[i].sensor_right);
      console.log("this.drones[", i, "]",".sensor_left:", this.drones[i].sensor_left);
      console.log("this.drones[", i, "]",".z_angle:", this.drones[i].z_angle);
      console.log("this.drones[", i, "]",".timestamp:", this.drones[i].timestamp);
    }
    console.log('\n');
    this.emitNewDronesInfo();
  }

  droneAlreadyInList(address: number) {
    for(let i = 0; i < this.drones.length; i++){
      if (address == this.drones[i].address)
        return true;
    }
    return false;
  }

  getBatteryLevel(index: number) {
    if(this.drones.length <= index) return 10000;
    return this.drones[index].drone_battery_level;
  }

  getDroneState(index: number) {
    if(this.drones.length <= index) return 10000;
    return this.drones[index].drone_state;
  }

  getTimeStamp(index: number) {
    if(this.drones.length <= index) return 'timestamp not available';
    return this.drones[index].timestamp;
  }

  emitNewDronesInfo(): void {
    this.messageEmitter.emit('updateInformation');
    this.mapEmitter.emit('updateMap');
  }

  getDroneIndex(address: number) {
    for(let i = 0; i < this.drones.length; i++){
      if(this.drones[i].address == address)
        return i;
    }
    return -1;
  }

  getDronePosition(index:number) {
    const x = this.drones[index].drone_position_x / 10;
    const y = this.drones[index].drone_position_y / 10;
    return [x, y];
  }

  getDroneEnvironment(index:number) {
    const front = this.drones[index].sensor_front / 10;
    const back = this.drones[index].sensor_back / 10;
    const right = this.drones[index].sensor_right / 10;
    const left = this.drones[index].sensor_left / 10;
    const angle = this.drones[index].z_angle;
    return [front, back, right, left, angle];
  }

}
