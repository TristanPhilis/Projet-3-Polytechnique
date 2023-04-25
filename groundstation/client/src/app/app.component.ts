import { Component, EventEmitter, OnInit } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { DroneService } from './services/drone.service';
import { Drone } from './interfaces/drone';
import { MatDialog } from '@angular/material/dialog';
import { LogViewerComponent } from './components/log-viewer/log-viewer.component';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  title = 'my-app';
  
  // shouldRun = /(^|.)(stackblitz|webcontainer).(io|com)$/.test(window.location.host);
  serverData: any;
  employeeData: any;
  
  constructor(private httpClient: HttpClient,
              private droneService: DroneService,
              public dialog: MatDialog,
              // public logViewerComp: LogViewerComponent
              ) {
  }
  
  ngOnInit() {
    setInterval(() => this.dronesUpdate(), 1000);
  }
  
  dronesUpdate() {
    this.httpClient.get('http://127.0.0.1:5000/dronesupdate').subscribe(data => {
      this.droneService.dronesUpdate(data as Drone[]);
    })
  }

  searchDrones() {
      this.httpClient.get('http://127.0.0.1:5000/scan').subscribe(data => {
      this.serverData = data as JSON;
      console.log(this.serverData);
    })
  }

  // // Embeded
  launchMissionDrone() {
    this.httpClient.get('http://127.0.0.1:5000/explore').subscribe(data => {
      this.serverData = data as JSON;
      console.log(this.serverData);
    })
  }

  endMissionDrone() {
    this.httpClient.get('http://127.0.0.1:5000/land').subscribe(data => {
      this.serverData = data as JSON;
      console.log(this.serverData);
    })
  }

  openDialog() {
    const dialogRef = this.dialog.open(LogViewerComponent, {
      height: '75%',
      width: '50%',
    });

    dialogRef.afterClosed().subscribe((result: any) => {
      console.log(`Dialog result: ${result}`);
    });
  }

  returnToBase() {
    this.httpClient.get('http://127.0.0.1:5000/returntobase').subscribe(data => {
      this.serverData = data as JSON;
      console.log(this.serverData);
    })
  }

}
