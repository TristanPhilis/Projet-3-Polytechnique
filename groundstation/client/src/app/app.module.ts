import { HttpClient, HttpClientModule } from '@angular/common/http';
import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import {MatButtonModule} from '@angular/material/button';
import {MatIconModule} from '@angular/material/icon';
import {MatToolbarModule} from '@angular/material/toolbar';
import {MatProgressSpinnerModule} from '@angular/material/progress-spinner'; 
import {MatDividerModule} from '@angular/material/divider';
import {MatDialogModule} from '@angular/material/dialog';
import {MatListModule} from '@angular/material/list'; 

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { SidebarComponent } from './components/sidebar/sidebar.component';
import { DroneInformationComponent } from './components/drone-information/drone-information.component';
import { LogViewerComponent } from './components/log-viewer/log-viewer.component';
import { MapComponent } from './components/map/map.component';
import { MapBoxComponent } from './components/map-box/map-box.component';

@NgModule({
  declarations: [
    AppComponent,
    SidebarComponent,
    DroneInformationComponent,
    LogViewerComponent,
    MapBoxComponent,
    MapComponent
  ],
  imports: [
    BrowserModule,
    BrowserAnimationsModule,
    AppRoutingModule,
    HttpClientModule,
    MatButtonModule,
    MatIconModule,
    MatToolbarModule,
    MatProgressSpinnerModule,
    MatDividerModule,
    MatDialogModule,
    MatListModule
  ],
  providers: [HttpClient, LogViewerComponent],
  bootstrap: [AppComponent]
})
export class AppModule { }
