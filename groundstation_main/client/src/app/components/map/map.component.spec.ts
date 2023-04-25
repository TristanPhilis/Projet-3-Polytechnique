import { ComponentFixture, TestBed } from '@angular/core/testing';
import { Drone } from 'src/app/interfaces/drone';
import { DroneService } from 'src/app/services/drone.service';
import { MapComponent } from './map.component';
import { HttpClient } from '@angular/common/http';
import { EventEmitter } from '@angular/core';
import { HttpClientTestingModule, HttpTestingController } from '@angular/common/http/testing';

describe('MapComponent', () => {
  let component: MapComponent;
  let fixture: ComponentFixture<MapComponent>;
  let httpClient: jasmine.SpyObj<HttpClient>;
  let droneService: jasmine.SpyObj<DroneService>;
  let httpTestingController: HttpTestingController;
  const messageEmitter: EventEmitter<string> = new EventEmitter<string>();

  const canvas = document.createElement('canvas');
  canvas.width = 100;
  canvas.height = 100;

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
    droneService = jasmine.createSpyObj('DroneService', ['getDroneIndex', 'getDronePosition', 'getDroneEnvironment']);
    httpClient = jasmine.createSpyObj('HttpClient', ['get']);
    await TestBed.configureTestingModule({
      imports: [HttpClientTestingModule],
      declarations: [ MapComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    httpTestingController = TestBed.inject(HttpTestingController);
    fixture = TestBed.createComponent(MapComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
    component.canvas = canvas;
    component.canvas.width = 100;
    component.canvas.height = 100;
    component.MapCtx = canvas.getContext('2d') as CanvasRenderingContext2D;
  });

  afterEach(() => {
    canvas.getContext('2d')?.clearRect(0, 0, canvas.width, canvas.height);
  })

  it('should create', () => {
    expect(component).toBeTruthy();
  });

  it('should update maps info', () => {
    
    const droneList: Drone[] = [];
    droneService.drones = droneList;
    component['droneService'] = droneService;

    // Test without the drone in the service's list
    droneService.getDroneIndex.and.returnValue(-1);
    component.updateMap();
    expect(droneService.getDronePosition).not.toHaveBeenCalled();
    expect(droneService.getDroneEnvironment).not.toHaveBeenCalled();
    
    
    // Test with the drone in the service's list
    spyOn(component, 'drawMap');
    droneService.getDroneIndex.and.callThrough();
    droneList.push(drone);
    component['droneService'].drones = droneList;
    component.lastPosition = [0, 0];
    component.updateMap();
    expect(component.drawMap).toHaveBeenCalled();
  });

  it('should convert degree to radian', () => {
    const randomDegree = Math.random();
    const radianConversion = randomDegree * Math.PI / 180;
    expect(component.degrees_to_radians(randomDegree)).toEqual(radianConversion);
  })

  it('should draw on the map', () => {
    spyOn(component.MapCtx, 'beginPath');
    const droneList: Drone[] = [];
    droneService.getDroneIndex.and.callThrough();
    droneList.push(drone);
    component['droneService'].drones = droneList;
    component.lastPosition = [0, 0];
    component.currentPositon = [drone.drone_position_x, drone.drone_position_y];
    component.surroundingValues = [drone.sensor_front, drone.sensor_back, drone.sensor_right, drone.sensor_left, drone.z_angle];
    component.mapSize = [canvas.width, canvas.height];
    component.drawMap();
    expect(component.MapCtx.beginPath).toHaveBeenCalled();
  });

});
