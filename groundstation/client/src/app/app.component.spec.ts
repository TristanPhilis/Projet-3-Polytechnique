import { fakeAsync, TestBed, tick } from '@angular/core/testing';
import { RouterTestingModule } from '@angular/router/testing';
import { AppComponent } from './app.component';
import { HttpClient } from '@angular/common/http';
import { MatToolbarModule } from '@angular/material/toolbar';
import { SidebarComponent } from './components/sidebar/sidebar.component';
import { CUSTOM_ELEMENTS_SCHEMA } from '@angular/core';
import { HttpClientTestingModule, HttpTestingController } from '@angular/common/http/testing';
import { of } from 'rxjs';
import { Drone } from './interfaces/drone';
import { DroneService } from './services/drone.service';
import { MatDialogModule } from '@angular/material/dialog';

describe('AppComponent', () => {
  let httpClient: jasmine.SpyObj<HttpClient>;
  let app: AppComponent;
  let httpTestingController: HttpTestingController;
  let droneService: jasmine.SpyObj<DroneService>;

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


  beforeEach(async () => {
    droneService = jasmine.createSpyObj('DroneService', ['dronesUpdate']);
    httpClient = jasmine.createSpyObj('HttpClient', ['get']);
    await TestBed.configureTestingModule({
      imports: [
        RouterTestingModule,
        MatToolbarModule,
        HttpClientTestingModule,
        MatDialogModule
      ],
      declarations: [
        AppComponent,
        SidebarComponent,
      ],
      providers: [
        { provide: HttpClient, useValue: httpClient }
      ],
      schemas: [CUSTOM_ELEMENTS_SCHEMA]
    }).compileComponents();
  });
  
  beforeEach(() => {
    
    httpTestingController = TestBed.inject(HttpTestingController);
    const fixture = TestBed.createComponent(AppComponent);
    app = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create the app', () => {
    expect(app).toBeTruthy();
  });


  it('should call updateDrone() from service', () => {
    droneService.dronesUpdate.and.callFake((drones: Drone[])=> {
      droneService.drones = drones;
    });
    // The drone list for dronesUpdate parameter 
    let droneList: Drone[] = [];
    droneList.push(drone);
    httpClient.get.and.returnValue(of(droneList));

    // Injecting services.
    app['droneService'] = droneService;
    app['httpClient'] = httpClient;
    // Calling the function.
    app.dronesUpdate();
    // Expectations
    expect(app['droneService'].dronesUpdate).toHaveBeenCalled();
    expect(app['droneService'].dronesUpdate).toHaveBeenCalledWith(droneList);
    expect(app['droneService'].drones).toEqual(droneList);
  });
  
  it('should call function scan', fakeAsync(() => {
    const fixture = TestBed.createComponent(AppComponent);
    const button = fixture.debugElement.nativeElement.querySelector('#scan');

    // // Injecting services.
    httpClient.get.and.returnValue(of('scan'));
    app['droneService'] = droneService;
    app['httpClient'] = httpClient;

    // The drone list for dronesUpdate parameter
    button.click();
    tick();

    expect(httpClient.get).toHaveBeenCalledWith('http://127.0.0.1:5000/scan');
  }));

  it('should call function launchMissionDrone', fakeAsync(() => {
    const fixture = TestBed.createComponent(AppComponent);
    const button = fixture.debugElement.nativeElement.querySelector('#explore');

    // // Injecting services.
    httpClient.get.and.returnValue(of('explore'));
    app['droneService'] = droneService;
    app['httpClient'] = httpClient;

    // The drone list for dronesUpdate parameter
    button.click();
    tick();

    expect(httpClient.get).toHaveBeenCalledWith('http://127.0.0.1:5000/explore');
  }));

  it('should call function launchMissionDrone', fakeAsync(() => {
    const fixture = TestBed.createComponent(AppComponent);
    const button = fixture.debugElement.nativeElement.querySelector('#land');

    // // Injecting services.
    httpClient.get.and.returnValue(of('land'));
    app['droneService'] = droneService;
    app['httpClient'] = httpClient;

    // The drone list for dronesUpdate parameter
    button.click();
    tick();

    expect(httpClient.get).toHaveBeenCalledWith('http://127.0.0.1:5000/land');
  }));

  it('should call function returnToBase', fakeAsync(() => {
    const fixture = TestBed.createComponent(AppComponent);
    const button = fixture.debugElement.nativeElement.querySelector('#returntobase');

    // // Injecting services.
    httpClient.get.and.returnValue(of('returntobase'));
    app['droneService'] = droneService;
    app['httpClient'] = httpClient;

    // The drone list for dronesUpdate parameter
    button.click();
    tick();

    expect(httpClient.get).toHaveBeenCalledWith('http://127.0.0.1:5000/returntobase');
  }));

  // ************************************************************************** //
  // The following test were there by default. They are commented because they make it fail
  // Please Do not consider it in Projet 3 Genie informatique

  // it(`should have as title 'client'`, () => {
  //   const fixture = TestBed.createComponent(AppComponent);
  //   const app = fixture.componentInstance;
  //   expect(app.title).toEqual('client');
  // });

  // it('should render title', () => {
  //   const fixture = TestBed.createComponent(AppComponent);
  //   fixture.detectChanges();
  //   const compiled = fixture.nativeElement as HTMLElement;
  //   expect(compiled.querySelector('.content span')?.textContent).toContain('client app is running!');
  // });
  // ************************************************************************** //
});
