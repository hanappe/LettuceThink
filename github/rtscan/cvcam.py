import cv2
import hal

class Camera(hal.Camera):
    def __init__(self, id, width, height):
        self.video = cv2.VideoCapture(id)
        if not self.video.isOpened():
            pass # throw error?
        self.video.set(3, width)
        self.video.set(4, height)
        self.width = self.video.get(3)
        self.height = self.video.get(4)
        
    def __del__(self):
        self.video.release()

    def get_resolution(self):
        return self.width, self.height

    def get_frame(self):
        check, frame = self.video.read()
        if check:
            return frame
        else:
            pass # throw error?
