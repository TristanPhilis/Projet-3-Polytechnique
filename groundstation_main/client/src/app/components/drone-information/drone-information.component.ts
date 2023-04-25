import { Component, OnInit, Input } from '@angular/core';
import { DroneService } from 'src/app/services/drone.service';
import { DroneState } from 'src/app/interfaces/droneState';
import {ThemePalette} from '@angular/material/core';
import {ProgressSpinnerMode} from '@angular/material/progress-spinner';
import { HttpClient } from '@angular/common/http';

@Component({
  selector: 'app-drone-information',
  templateUrl: './drone-information.component.html',
  styleUrls: ['./drone-information.component.scss']
})
export class DroneInformationComponent implements OnInit {
  @Input() address: string = "";
  @Input() addressNumber: number = 0;
  drone_battery_level: number = 0;
  state: string = "IDLE";
  timestamp: string = "";
  color: ThemePalette = 'primary';
  mode: ProgressSpinnerMode = 'determinate';
  serverData: any;

  constructor(private httpClient: HttpClient, private droneService: DroneService) {}

  ngOnInit(): void {
    this.droneService.messageEmitter.subscribe(() => {
      this.updateInfo();
    })
  }

  getDroneStateEnumIndex(state: string): number{
    return Object.values(DroneState).indexOf(state as DroneState);
  }
  getState(state: number): string {
    switch(state){
      case this.getDroneStateEnumIndex(DroneState.STATE_IDLE):
        return DroneState.STATE_IDLE;
      case this.getDroneStateEnumIndex(DroneState.STATE_TAKE_OFF):
        return DroneState.STATE_TAKE_OFF;
      case this.getDroneStateEnumIndex(DroneState.STATE_EXPLORATION):
        return DroneState.STATE_EXPLORATION;
      case this.getDroneStateEnumIndex(DroneState.STATE_RETURN_HOME):
        return DroneState.STATE_RETURN_HOME;
      case this.getDroneStateEnumIndex(DroneState.STATE_LANDING):
        return DroneState.STATE_LANDING;
      case this.getDroneStateEnumIndex(DroneState.STATE_CRASHED):
        return DroneState.STATE_CRASHED;
    }

    return "Non defini";
  }

  updateInfo() {
    console.log('Informationing');
    const index = this.droneService.getDroneIndex(this.addressNumber);
    if (index == -1)
      return;
    this.drone_battery_level = this.droneService.getBatteryLevel(index);
    this.state = this.getState(this.droneService.getDroneState(index));
    this.timestamp = this.droneService.getTimeStamp(index);
  }

  identifyDrone() {
    this.httpClient.get('http://127.0.0.1:5000/identifyonedrone/' + this.addressNumber).subscribe(data => {
      this.serverData = data as JSON;
      console.log(this.serverData);
    })
  }
}
