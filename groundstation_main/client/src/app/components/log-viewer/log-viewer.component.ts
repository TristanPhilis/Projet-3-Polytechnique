import { Component, OnInit, ViewChild } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { DroneService } from 'src/app/services/drone.service';

@Component({
  selector: 'app-log-viewer',
  templateUrl: './log-viewer.component.html',
  styleUrls: ['./log-viewer.component.scss']
})
export class LogViewerComponent implements OnInit {

  constructor(private httpClient: HttpClient, private droneService: DroneService) { }

  ngOnInit(): void {
    this.fetchLogNames();
  }

  @ViewChild('textdisp')textDisplay: any;
  typesOfShoes: string[] = ['Boots', 'Clogs', 'Loafers', 'Moccasins', 'Sneakers'];
  logNameArray: any;
  currentLog: any;
  reader = new FileReader();

  fetchLogNames(): void {
    this.httpClient.get('http://127.0.0.1:5000/fetchLogNames').subscribe((data: any) => {
      this.logNameArray = data as JSON;
    })
  }

  fetchLog(name: string): void {
    var text = this.textDisplay.nativeElement;
    this.reader.addEventListener("load", function () {
      text.src = "";
      text.src = this.result;
    }, false);

    this.httpClient.get('http://127.0.0.1:5000/sendfile/' + name, {responseType: 'blob'})
    .subscribe((data: any) => {
      this.currentLog = new File([data], name, {type: "text/plain"});
      this.reader.readAsDataURL(this.currentLog);
    })
  }
}
