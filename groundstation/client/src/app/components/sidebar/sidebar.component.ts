import { Component, OnInit } from '@angular/core';
import { DroneService } from 'src/app/services/drone.service';

@Component({
  selector: 'app-sidebar',
  templateUrl: './sidebar.component.html',
  styleUrls: ['./sidebar.component.scss']
})
export class SidebarComponent implements OnInit {

  constructor(private droneService: DroneService) { }

  ngOnInit(): void {
  }

  getAddresses() {
    let addresses: number[] = [];
    for(let i=0; i < this.droneService.drones.length; i++){
      addresses.push(this.droneService.drones[i].address);
    }
    return addresses;
  }
}
