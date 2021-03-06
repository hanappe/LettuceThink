import serial
import json
import time
import hal

# cmp() is no longer defined in Python3 (silly)
def cmp(a, b):
    return (a > b) - (a < b)

class CNC(hal.CNC):

    def __init__(self, port="/dev/ttyUSB0", homing=True):
        self.port = port
        self.serial_port = serial.Serial(self.port, 115200)
        # TODO: read '#ready' ?
        while self.serial_port.in_waiting == 0:
            time.sleep(0.1)   
        r = self.serial_port.readline() 
        if homing: self.home()
        self.set_zero()
        self.status = "idle"
        self.p = [0, 0, 0]
        self.v = [0, 0, 0]
        self.update_status()
        

    def home(self):
        self.__send('h')

        
    def has_position_control():
        return True

    
    def get_position(self):
        self.update_status()
        return self.p

    
    def has_velocity_control():
        return True

    
    def get_velocity(self):
        self.update_status()
        return self.v

    
    def set_zero(self):
        self.__send('0')

        
    def start_spindle(self, ):
        self.__send('S1')

                
    def stop_spindle(self, ):
        self.__send('S0')

        
    def get_status(self):
        self.update_status()
        return self.status

    
    
    def moveat(self, vx, vy, vz):
        self.__send("x%d" % vx)
        self.__send("y%d" % vy)
        self.__send("z%d" % vz)

        
    def set_target_pos(self, x, y, z):
        self.__send("X%d" % x)
        self.__send("Y%d" % y)
        self.__send("Z%d" % z)


    # Dont use, doesn't work yet
    def __wait__(self):
        self.__send("W")

    def wait(self):
        time.sleep(0.1)   
        self.update_status()
        while self.status == "moving":
            time.sleep(0.1)   
            self.update_status()

        
    def moveto(self, x, y, z, v):
        self.__send("X%d" % x)
        self.__send("Y%d" % y)
        self.__send("Z%d" % z)
        self.__send("M%d" % v)

        
    def moveto(self, x, y, z):
        self.moveto(x, y, z, 50)

        
    def moveto(self, p, v):
        self.set_target_pos(p[0], p[1], p[2])
        self.moveat(v[0], v[1], v[2])

                
    def moveto_z(self, z, vz):
        self.__send("Z%d" % z)
        self.__send("z%d" % vz)

        
            
    def update_status(self):
        s = self.__send("s")
        try:
            stat = json.loads(s.decode("utf-8"))
            self.status = stat["status"]
            self.p = stat["p"]
            self.v = stat["v"]
        except KeyError as k:
            print("Failed to parse the JSON: missing key")
        except Exception as e:
            print("Failed to parse the JSON: %s" % str(e))
            print("String was: %s" % s)
        finally:
            pass # dummy statement to avoid empty 'finally' clause
        #print('status=%s, p=%s, v=%s' % (self.status, str(self.p), str(self.v)))

        
    def __send(self, s):
        r = False
        try:
            self.serial_port.write(bytes('%s\n' % s, 'utf-8'))
            time.sleep(0.01)   
            r = self.serial_port.readline()
        finally:
            pass # dummy statement to avoid empty 'finally' clause
        if r == False:
            print('cmd=%s: failed' % (s))
        return r;

    
if __name__ == "__main__":
    cnc = CNCVelocityControl("/dev/ttyACM0")
    rounds = 3
    xoff = 0
    for round in range(rounds):
        cnc.moveto([xoff, -600, 0], [0, -50, 0])
        cnc.moveto([xoff - 50, -600, 0], [-20, 0, 0])
        cnc.moveto([xoff - 50, 10, 0], [0, 50, 0])
        if (round < rounds - 1):
            cnc.moveto([xoff - 100, 10, 0], [-20, 0, 0])
        xoff -= 100


