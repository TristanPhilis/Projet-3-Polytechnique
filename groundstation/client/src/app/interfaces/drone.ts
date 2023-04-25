export interface Drone {
    address: number;
    drone_position_x: number;
    drone_position_y: number;
    drone_battery_level: number;
    drone_state: number;
    sensor_front: number;
    sensor_back: number;
    sensor_right: number;
    sensor_left: number;
    z_angle: number;
    timestamp: string;
}