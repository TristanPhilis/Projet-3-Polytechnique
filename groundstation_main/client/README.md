# Docker
## Build docker image before launching client
-Go into "client" folder and open a terminal window  
-Enter the following command  
-"docker build -t client-app ."  

Docker image name in this case : client-app (name can be freely chosen)  

## Run docker container
-Remain in the same terminal window  
-Enter the following command  
-"docker run -it --rm -p 9898:4200 --name client-container client-app"  
-Type the following address in a web browser to access client  
-"http://localhost:9898/"  

Docker container name : client-container (name can be freely chosen)  

# Client

This project was generated with [Angular CLI](https://github.com/angular/angular-cli) version 12.2.7.

## Development server

Run `ng serve` for a dev server. Navigate to `http://localhost:4200/`. The app will automatically reload if you change any of the source files.

## Code scaffolding

Run `ng generate component component-name` to generate a new component. You can also use `ng generate directive|pipe|service|class|guard|interface|enum|module`.

## Build

Run `ng build` to build the project. The build artifacts will be stored in the `dist/` directory.

## Running unit tests

Run `ng test` to execute the unit tests via [Karma](https://karma-runner.github.io).

## Running end-to-end tests

Run `ng e2e` to execute the end-to-end tests via a platform of your choice. To use this command, you need to first add a package that implements end-to-end testing capabilities.

## Further help

To get more help on the Angular CLI use `ng help` or go check out the [Angular CLI Overview and Command Reference](https://angular.io/cli) page.
