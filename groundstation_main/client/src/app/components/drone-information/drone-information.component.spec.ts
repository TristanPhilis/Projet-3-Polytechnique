import { EventEmitter } from '@angular/core';
import { ComponentFixture, fakeAsync, TestBed, tick } from '@angular/core/testing';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { Drone } from 'src/app/interfaces/drone';
import { DroneState } from 'src/app/interfaces/droneState';
import { DroneService } from 'src/app/services/drone.service';
import { HttpClientTestingModule, HttpTestingController } from '@angular/common/http/testing';

import { DroneInformationComponent } from './drone-information.component';
import { of } from 'rxjs';
import { HttpClient } from '@angular/common/http';

describe('DroneInformationComponent', () => {
  let component: DroneInformationComponent;
  let fixture: ComponentFixture<DroneInformationComponent>;
  let httpClient: jasmine.SpyObj<HttpClient>;
  let droneService: jasmine.SpyObj<DroneService>;
  let httpTestingController: HttpTestingController;
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

  beforeEach(async () => {
    droneService = jasmine.createSpyObj('DroneService', ['getDroneIndex', 'getBatteryLevel', 'getDroneState', 'getTimeStamp', 'emitNewDronesInfo']);
    httpClient = jasmine.createSpyObj('HttpClient', ['get']);
    await TestBed.configureTestingModule({
      imports: [MatProgressSpinnerModule, HttpClientTestingModule],
      declarations: [ DroneInformationComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    httpTestingController = TestBed.inject(HttpTestingController);
    fixture = TestBed.createComponent(DroneInformationComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });

  it('should update info', () => {
    const droneList: Drone[] = [];
    droneService.drones = droneList;
    component['droneService'] = droneService;

    // Test without the drone in the service's list
    droneService.getDroneIndex.and.returnValue(-1);
    component.updateInfo();
    expect(droneService.getBatteryLevel).not.toHaveBeenCalled();
    
    
    // Test with the drone in the service's list
    droneService.getDroneIndex.and.callThrough();
    droneList.push(drone);
    component['droneService'].drones = droneList;
    component.updateInfo();
    expect(component['droneService'].getBatteryLevel).toHaveBeenCalled();
  });

  it('should call updateInfo', () => {
    droneService.messageEmitter = messageEmitter;
    droneService.emitNewDronesInfo.and.callFake(()=>{
      droneService.messageEmitter.emit('updateInformation');
    })
    spyOn(droneService.messageEmitter, 'emit').and.callThrough();
    component['droneService'] = droneService;
    spyOn(component, 'updateInfo').and.callFake(()=> { return; });
    
    // Test
    component.ngOnInit(); 
    component['droneService'].emitNewDronesInfo();
    expect(component['updateInfo']).toHaveBeenCalled();
    expect(component['droneService'].messageEmitter.emit).toHaveBeenCalledWith('updateInformation');
  });

  it('should call updateInfo', () => {
    const STATE_IDLE = 0;
    const STATE_TAKE_OFF = 1;
    const STATE_EXPLORATION = 2;
    const STATE_RETURN_HOME = 3;
    const STATE_LANDING = 4;
    const STATE_CRASHED = 5;

    expect(component.getState(STATE_IDLE)).toEqual(DroneState.STATE_IDLE);
    expect(component.getState(STATE_TAKE_OFF)).toEqual(DroneState.STATE_TAKE_OFF);
    expect(component.getState(STATE_EXPLORATION)).toEqual(DroneState.STATE_EXPLORATION);
    expect(component.getState(STATE_RETURN_HOME)).toEqual(DroneState.STATE_RETURN_HOME);
    expect(component.getState(STATE_LANDING)).toEqual(DroneState.STATE_LANDING);
    expect(component.getState(STATE_CRASHED)).toEqual(DroneState.STATE_CRASHED);
  });

  it('should call function identify', fakeAsync(() => {
    // const fixture = TestBed.createComponent(DroneInformationComponent);
    const button = fixture.debugElement.nativeElement.querySelector('#identifyonedrone');

    // // Injecting services.
    httpClient.get.and.returnValue(of('identifyonedrone'));
    component['droneService'] = droneService;
    component['httpClient'] = httpClient;

    // The drone list for dronesUpdate parameter
    button.click();
    tick();

    expect(httpClient.get).toHaveBeenCalledWith('http://127.0.0.1:5000/identifyonedrone/' + component.addressNumber);
  }));
});
