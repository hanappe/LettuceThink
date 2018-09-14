
class CNC(object):
    def __init__(self):
        pass
    
    def home(self):
        pass
    
    def has_position_control():
        pass
    
    def get_position(self):
        pass
    
    def moveto(self, x, y, z):
        pass

    def wait(self):
        pass

    def has_velocity_control():
        pass
    
    def get_velocity(self):
        pass

    def moveat(self, vx, vy, vz):
        pass


    
class Gimbal(object):
    def __init__(self):
        pass
    
    def has_position_control():
        pass
    
    def get_position(self):
        pass
    
    def moveto(self, pan, tilt):
        pass

    def wait(self):
        pass

    def has_velocity_control():
        pass
    
    def get_velocity(self):
        pass

    def moveat(self, vpan, vtilt):
        pass

    
class Camera(object):
    def __init__(self):
        pass

    def get_resolution(self):
        pass

    def get_frame(self):
        pass
    
    
class GameController(object):
    def __init__(self):
        pass

    def set_callback(self, name, callback):
        pass

    def handle_events(self):
        pass

    
        
class Scanner(object):
    def __init__(self, cnc, gimbal, camera):
        self.cnc = cnc
        self.gimbal = gimbal
        self.camera = camera


    
