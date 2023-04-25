from enum import Enum

class DroneState(Enum):
    STATE_IDLE = 0
    STATE_TAKE_OFF = 1
    STATE_EXPLORATION = 2
    STATE_RETURN_HOME = 3
    STATE_LANDING = 4
    STATE_CRASHED = 5