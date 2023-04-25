import { AfterViewInit, Component, ElementRef, OnInit, Input, ViewChild} from '@angular/core';
import { DroneService } from 'src/app/services/drone.service';
import {ThemePalette} from '@angular/material/core';
import {ProgressSpinnerMode} from '@angular/material/progress-spinner';

@Component({
  selector: 'app-drone-map',
  templateUrl: './map.component.html',
  styleUrls: ['./map.component.scss']
})
export class MapComponent implements OnInit, AfterViewInit {
  @ViewChild('mapCanvas', {static: true,}) mapCanvas: ElementRef;

  canvas: HTMLCanvasElement;
  MapCtx: CanvasRenderingContext2D;

  mapSize: number[] = [];
  @Input() address: string = "";
  @Input() addressNumber: number = 0;
  lastPosition: number[] = [];
  currentPositon: number[] = [];
  surroundingValues: number[] = [];
  front = 0;
  back = 1;
  right = 2;
  left = 3;
  angle = 4;

  constructor(private droneService: DroneService) { 
    // this.getBatteryLevel()
   }

  ngOnInit(): void {
    this.droneService.mapEmitter.subscribe(() => {
      this.updateMap();
    })
  }

  ngAfterViewInit(){
    this.canvas = this.mapCanvas.nativeElement;
    this.MapCtx = this.mapCanvas.nativeElement.getContext('2d') as CanvasRenderingContext2D;

    this.mapSize[0] = this.canvas.width;
    this.mapSize[1] = this.canvas.height;

    this.MapCtx.lineWidth = 3;
    this.MapCtx.lineCap = 'round';
    this.MapCtx.strokeStyle = '#000';

    this.updateMap();
  }

  degrees_to_radians(degrees: number){
  const pi = Math.PI;
  return degrees * (pi/180);
  }

  updateMap() {
    const index = this.droneService.getDroneIndex(this.addressNumber);
    if (index == -1)
      return;
    if(this.currentPositon){
      this.lastPosition = this.currentPositon;
      this.currentPositon = this.droneService.getDronePosition(index);
    } else {
      this.currentPositon = this.droneService.getDronePosition(index);
    }
    this.surroundingValues = this.droneService.getDroneEnvironment(index)
    this.drawMap();
    
  }

  drawMap(){
    const halfMapWidth = this.mapSize[0] / 2;
    const halfMapHeight = this.mapSize[1] /2;

    const frontAngleRad = this.degrees_to_radians(this.surroundingValues[this.angle]);
    const rightAngleRad = this.degrees_to_radians(this.surroundingValues[this.angle] - 90);
    const backAngleRad = this.degrees_to_radians(this.surroundingValues[this.angle] + 180);
    const leftAngleRad = this.degrees_to_radians(this.surroundingValues[this.angle] + 90);

    //Clear last drone position
    this.MapCtx.lineWidth = 5;
    this.MapCtx.strokeStyle = 'white';
    this.MapCtx.beginPath();
    this.MapCtx.moveTo((this.lastPosition[0] / 1000) * this.mapSize[0] + halfMapWidth, (this.lastPosition[1] / 1000) * this.mapSize[1] + halfMapHeight);
    this.MapCtx.lineTo((this.lastPosition[0] / 1000) * this.mapSize[0] + halfMapWidth, (this.lastPosition[1] / 1000) * this.mapSize[1] + halfMapHeight);
    this.MapCtx.stroke();
    this.MapCtx.closePath();

    //Draw  red dot at the positon of the drone
    this.MapCtx.lineWidth = 3;
    this.MapCtx.strokeStyle = 'red';
    this.MapCtx.beginPath();
    this.MapCtx.moveTo((this.currentPositon[0] / 1000) * this.mapSize[0] + halfMapWidth, (this.currentPositon[1] / 1000) * this.mapSize[1] + halfMapHeight);
    this.MapCtx.lineTo((this.currentPositon[0] / 1000) * this.mapSize[0] + halfMapWidth, (this.currentPositon[1] / 1000) * this.mapSize[1] + halfMapHeight);
    this.MapCtx.stroke();
    this.MapCtx.closePath();

    //Get surrounding environment values taken by sensors
    const frontX = (this.currentPositon[0] / 1000) * this.mapSize[0] + halfMapWidth + (this.surroundingValues[this.front] / 1000) * this.mapSize[0] * Math.cos(frontAngleRad);
    const backX = (this.currentPositon[0] / 1000) * this.mapSize[0] + halfMapWidth + (this.surroundingValues[this.back] / 1000) * this.mapSize[0] * Math.cos(backAngleRad);
    const rightX = (this.currentPositon[0] / 1000) * this.mapSize[0] + halfMapWidth + (this.surroundingValues[this.right] / 1000) * this.mapSize[0] * Math.cos(rightAngleRad);
    const leftX = (this.currentPositon[0] / 1000) * this.mapSize[0] + halfMapWidth + (this.surroundingValues[this.left] / 1000) * this.mapSize[0] * Math.cos(leftAngleRad);

    const frontY = (this.currentPositon[1] / 1000) * this.mapSize[1] + halfMapHeight + (this.surroundingValues[this.front] / 1000) * this.mapSize[1] * Math.sin(-frontAngleRad);
    const backY = (this.currentPositon[1] / 1000) * this.mapSize[1] + halfMapHeight + (this.surroundingValues[this.back] / 1000) * this.mapSize[1] * Math.sin(-backAngleRad);
    const rightY = (this.currentPositon[1] / 1000) * this.mapSize[1] + halfMapHeight + (this.surroundingValues[this.right] / 1000) * this.mapSize[1] * Math.sin(-rightAngleRad);
    const leftY = (this.currentPositon[1] / 1000) * this.mapSize[1] + halfMapHeight + (this.surroundingValues[this.left] / 1000) * this.mapSize[1] * Math.sin(-leftAngleRad);

    const noise = 20;
    this.MapCtx.strokeStyle = 'black';
    //Draw what sensor see at the front
    if(this.surroundingValues[this.front] > noise){
      this.MapCtx.beginPath();
      this.MapCtx.moveTo(frontX, frontY);
      this.MapCtx.lineTo(frontX, frontY);
      this.MapCtx.stroke();
      this.MapCtx.closePath();
    }

    //Draw what sensor see at the back
    if(this.surroundingValues[this.back] > noise){
      this.MapCtx.beginPath();
      this.MapCtx.moveTo(backX, backY);
      this.MapCtx.lineTo(backX, backY);
      this.MapCtx.stroke();
      this.MapCtx.closePath();
    }
    
    //Draw what sensor see at the left
    if(this.surroundingValues[this.left] > noise){
      this.MapCtx.beginPath();
      this.MapCtx.moveTo(leftX, leftY);
      this.MapCtx.lineTo(leftX, leftY);
      this.MapCtx.stroke();
      this.MapCtx.closePath();
    }

    //Draw what sensor see at the right
    if(this.surroundingValues[this.right] > noise){
      this.MapCtx.beginPath();
      this.MapCtx.moveTo(rightX, rightY);
      this.MapCtx.lineTo(rightX, rightY);
      this.MapCtx.stroke();
      this.MapCtx.closePath();
    }
  }
}
