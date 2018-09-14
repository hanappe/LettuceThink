import pyxl430
import hal

class Gimbal(hal.Gimbal):
    def __init__(self, dev = "/dev/ttyUSB1", mode = "velocity"):
        self.port = pyxl430.USB2Dynamixel(dev)
        self.port.start(1000000) #Start USB serial connection
        self.mode = mode
        
        self.pan = pyxl430.Actuator(self.port, 1) # get the motor with id 1
        self.pan.set_torque_enable(False) # deactivate motor
        if self.mode == "velocity":
            self.pan.set_operating_mode(1) # set wheel mode (velocity)
        # elif self.mode == "position":
        self.pan.set_profile_acceleration(32760)
        self.pan.set_torque_enable(True) # activate motor
        
        self.tilt = pyxl430.Actuator(self.port, 2) # get the motor with id 2
        self.tilt.set_torque_enable(False) # deactivate motor
        if self.mode == "velocity":
            self.tilt.set_operating_mode(1) # set wheel mode (velocity)
        # elif self.mode == "position":
        self.tilt.set_torque_enable(True) # activate motor

        
    def has_position_control():
        # TODO: easy to add
        return self.mode == "position"

    
    def get_position(self):
        pan = self.pan.get_present_position()
        tilt = self.tilt.get_present_position()
        return [360.0 * pan / 4096, 360.0 * tilt / 4096]

    
    def moveto(self, pan, tilt):
        # TODO
        #self.pan.set_goal_position(int(pan))
        #self.tilt.set_goal_position(int(tilt))
        pass

    
    def wait(self):
        # TODO
        # while self.pan.is_moving() or self.tilt.is_moving: ...
        pass

    
    def has_velocity_control():
        return self.mode == "velocity"

    
    def get_velocity(self):
        vpan = self.pan.get_present_velocity()
        vtilt = self.tilt.get_present_velocity()
        return vpan, vtilt

    
    def moveat(self, vpan, vtilt):
        self.pan.set_goal_velocity(int(vpan))
        self.tilt.set_goal_velocity(int(vtilt))

        

    
def set_boud_rate(rate, dev = "/dev/ttyUSB1"):
    usb = pyxl430.USB2Dynamixel(dev)
    usb.start() #Start USB serial connection

    pan = pyxl430.Actuator(usb, 1) # get the motor with id 1
    pan.set_torque_enable(False) # deactivate motor
    print("baud rate %d" % pan.get_baud_rate())
    pan.set_baud_rate(v)

    tilt = pyxl430.Actuator(usb, 2) # get the motor with id 2
    tilt.set_torque_enable(False) # deactivate motor
    print("baud rate %d" % tilt.get_baud_rate())
    tilt.set_baud_rate(v)

    
def get_boud_rate(rate, dev = "/dev/ttyUSB1"):
    usb = pyxl430.USB2Dynamixel(dev)
    # Start USB serial connection
    #usb.start(0) 
    usb.start(1000000)
    #usb.start(57600)

    pan = pyxl430.Actuator(usb, 1) # get the motor with id 1
    pan.set_torque_enable(False) # deactivate motor
    print("baud rate (pan): %d" % pan.get_baud_rate())

    tilt = pyxl430.Actuator(usb, 2) # get the motor with id 2
    tilt.set_torque_enable(False) # deactivate motor
    print("baud rate (tilt): %d" % tilt.get_baud_rate())
