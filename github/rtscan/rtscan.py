import hal
import jslinux
import dynamixel
import cnccontroller
import cvcam
import math
from time import sleep
import cv2
#import lettucethink.cv
import numpy as np


vx = 0
vy = 0
vpan = 0
vtilt = 0
frame = None
image_number = 0

def update_speed_and_direction():
    global vx, vy
    #currentPan, currentTilt = 1024, -1024
    currentPan, currentTilt = scanner.gimbal.get_position()
    alpha = math.pi / 2 - currentPan * math.pi / 180.0 
    beta = math.pi / 2 + currentTilt * math.pi / 180.0
    s = math.sin(alpha)
    c = math.cos(alpha)
    _vx = s * vy + c * vx 
    _vy = c * vy - s * vx
    #print("pan %f, tilt %f, v=(%f, %f)" % (180.0 * alpha / math.pi, 180.0 * beta / math.pi, vx, vy))
    scanner.cnc.moveat(_vx, _vy, 0)

    
def move_forward(name, value):
    global vx, vy
    if math.fabs(value) > 3000:
        vy = -50 * value / 32767.0
    else:
        vy = 0

        
def move_sideways(name, value):
    global vx, vy
    if math.fabs(value) > 3000:
        vx = -50 * value / 32767.0
    else:
        vx = 0

        
def look_left(name, value):
    if value: vpan = -15
    else: vpan = 0
    scanner.gimbal.moveat(vpan, vtilt)

    
def look_right(name, value):
    if value: vpan = 15
    else: vpan = 0
    scanner.gimbal.moveat(vpan, vtilt)

    
def look_down(name, value):
    if value: vtilt = -15
    else: vtilt = 0
    scanner.gimbal.moveat(vpan, vtilt)

    
def look_up(name, value):
    if value: vtilt = 15
    else: vtilt = 0
    scanner.gimbal.moveat(vpan, vtilt)

    
def look_leftright(name, value):
    if math.fabs(value) > 3000:
        vpan = 15 * value / 32767.0
    else:
        vpan = 0
    scanner.gimbal.moveat(vpan, vtilt)


def look_updown(name, value):
    if math.fabs(value) > 3000:
        vtilt = 15 * value / 32767.0
    else:
        vtilt = 0
    scanner.gimbal.moveat(vpan, vtilt)

    
def save_frame(name, value):
    global frame, image_number, scanner
    if value == 0:
        pos = scanner.cnc.get_position()
        angles = scanner.gimbal.get_position()
        name = "image-%02d.jpg" % image_number
        print("%f %f %f %f %s" % (pos[0], pos[1], angles[0] - 90, 90.0 + angles[1], name))
        cv2.imwrite(name, frame)
        image_number += 1

gimbal = dynamixel.Gimbal("/dev/ttyUSB1")
cnc = cnccontroller.CNC("/dev/ttyUSB0", False)
camera = cvcam.Camera(1, 640, 480)

scanner = hal.Scanner(cnc, gimbal, camera)

controller = jslinux.GameController("/dev/input/js0")
controller.set_callback("x", move_sideways)
controller.set_callback("y", move_forward)
 # For Logitech controller
controller.set_callback("top", look_left)
controller.set_callback("top2", look_right)
controller.set_callback("thumb", look_up)
controller.set_callback("thumb2", look_down)
controller.set_callback("trigger", save_frame)
# For PS3 controller
controller.set_callback("rx", look_leftright)
controller.set_callback("ry", look_updown)



while True:
    controller.handle_events()
    update_speed_and_direction()
    frame = scanner.camera.get_frame()
    
    #ExG = lettucethink.cv.calculateExcessGreen(frame)
    #M = ExG.max()
    #m = ExG.min()
    #ExGNorm = (255 * (ExG - m) / (M - m)).astype(np.uint8)
    #ExGNorm = cv2.bilateralFilter(ExGNorm, 11, 5, 17)

    #mask = lettucethink.cv.calculatePlantMask(frame, 0, False)

    cv2.imshow('frame', frame)
    #cv2.imshow('frame', mask)
    #cv2.imshow('frame', ExGNorm)
    
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    
cv2.destroyAllWindows()

