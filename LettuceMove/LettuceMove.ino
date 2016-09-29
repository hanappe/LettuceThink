#include <AccelStepper.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

int ENABLE = 5;
int DIR = 6;
int STEP = 7;

int stepper_accel = 2000;
float steps_per_millimeter = 22.8571f;

AccelStepper stepper(1, STEP, DIR);
BridgeServer server;

void setup()
{  
  Serial.begin(9600);

  Bridge.begin();

  int stepper_speed = 4500;
  stepper.setMaxSpeed(stepper_speed);
  stepper.setAcceleration(stepper_accel);
  stepper.setSpeed(stepper_speed);  
  stepper.setEnablePin(ENABLE);
  stepper.disableOutputs();

  server.listenOnLocalhost();
  server.begin();

  Serial.println("Weady"); 
}

void moveRobot(int millimeter)
{
  stepper.enableOutputs();
  long pos = stepper.currentPosition() + (long) ((float) millimeter * steps_per_millimeter);
  stepper.moveTo(pos);
  Serial.println(millimeter * steps_per_millimeter);
  
}

void stopRobot()
{
  stepper.stop();  
  stepper.disableOutputs();
}

void printInfo()
{
  Serial.print("pos: "); Serial.println(stepper.currentPosition());
  Serial.print("target: "); Serial.println(stepper.targetPosition());
  Serial.print("speed: "); Serial.println(stepper.maxSpeed());
  Serial.print("accel: "); Serial.println(stepper_accel);
}

void process(BridgeClient client) 
{
  String command = client.readStringUntil('\r');
  if (command == "forward") {
    moveRobot(300);
  } else if (command == "backward") {
    moveRobot(-300);
  } else if (command == "stop") {
    stopRobot();
  }
}

unsigned long lasttime = 0;

void loop()
{  
  stepper.run();
  if (!stepper.isRunning())
    stepper.disableOutputs();

  unsigned long time = millis();
  if (time - lasttime > 250) {
    lasttime = time;
    BridgeClient client = server.accept();
    if (client) {
      process(client);
      client.stop();
    }
  }

  // Commands:
  //
  // f1000: move forward 1000 "units"
  // b2000: move backward 1000 "units"
  // x: stop
  // s2000: set speed and maximum speed to 2000
  // a500: set acceleration to 500
  // p: print settings and position
  //
  while (Serial.available() > 0) {
    int c = Serial.read();
    if (c == 'f') {
      int mm = Serial.parseInt();
      moveRobot(mm);
    } else if (c == 'b') {
      int mm = Serial.parseInt();
      moveRobot(-mm);
    } else if (c == 's') {
      int speed = Serial.parseInt();
      stepper.setMaxSpeed(speed);
      stepper.setSpeed(speed);  
    } else if (c == 'a') {
      stepper_accel = Serial.parseInt();
      stepper.setAcceleration(stepper_accel);  
    } else if (c == 'x') {
      stopRobot();
    } else if (c == 'p') {
      printInfo();  
    }
  }
}


