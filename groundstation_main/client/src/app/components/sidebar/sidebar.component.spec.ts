import { CUSTOM_ELEMENTS_SCHEMA } from '@angular/core';
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { MatDividerModule } from '@angular/material/divider'; 
import { Drone } from 'src/app/interfaces/drone';
import { DroneService } from 'src/app/services/drone.service';

import { SidebarComponent } from './sidebar.component';

describe('SidebarComponent', () => {
  let component: SidebarComponent;
  let fixture: ComponentFixture<SidebarComponent>;
  let droneService: jasmine.SpyObj<DroneService>;
  
  beforeEach(async () => {
    droneService = jasmine.createSpyObj('DroneService', ['getBatteryLevel']);
    await TestBed.configureTestingModule({
      imports: [MatDividerModule],
      declarations: [ SidebarComponent ],
      schemas: [CUSTOM_ELEMENTS_SCHEMA]
    })
    .compileComponents();
  });

  beforeEach(() => {

    fixture = TestBed.createComponent(SidebarComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });

  it('should return a drone adress', () => {
    // A fake drone.
    const drone = {address: 123,
      drone_position_x: 123,
      drone_position_y: 123,
      drone_battery_level: 123,
      drone_state: 123,
      sensor_front: 123,
      sensor_back: 123,
      sensor_right: 123,
      sensor_left: 123,
      z_angle: 123,
      timestamp: "2021-12-8"
    } as Drone;
    droneService.drones = [];
    droneService.drones.push(drone);
    // injecting the service.
    component['droneService'] = droneService;
    component.getAddresses();
    expect(component['droneService'].drones[0].address).toEqual(drone.drone_battery_level);
  });
});
