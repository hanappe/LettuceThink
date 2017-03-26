#include <AccelStepper.h>

#define DIRECTION_DIR 5
#define DIRECTION_STEP 6
#define DIRECTION_ENABLE 13
#define encoder0PinA  2
#define encoder0PinB  4

volatile long encoder0Pos = 0;
AccelStepper stepper(AccelStepper::DRIVER,  DIRECTION_STEP,  DIRECTION_DIR);

void setup()
{ 
  Serial.begin(115200);
  stepper.setMaxSpeed(1800);
  stepper.setAcceleration(3000);

  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);       // turn on pull-up resistor
  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);       // turn on pull-up resistor
  EICRA |= 0x03;  //select rising edge on INT0 (digital 2)
  EIMSK |= 0x01;  //enable INT0 interrupts
}

#define MAX_STEPS 9000

int currentAngle = 0;
unsigned long lastRead = 0;


void loop()
{
  unsigned long now = millis();
  if (now - lastRead > 20) {
    lastRead = now;
    int targetAngle = 2 * map(analogRead(A0), 0, 1023, -45, 45);
    if (abs(targetAngle - currentAngle) > 1) {
      long pos = map(targetAngle, -90, 90, -MAX_STEPS, MAX_STEPS);
      stepper.moveTo(pos);
//      Serial.print(targetAngle);
//      Serial.print(' ');
//      Serial.println(currentAngle);
    }
  }
  stepper.run();
  //  Serial.println(encoder0Pos);

}

SIGNAL(INT0_vect)
{
  //Port D pin 2 is high since this is a rising interrupt
  if (PIND & 0x10)    //if pin 4 is also high
    encoder0Pos++;    //encoder is moving forward
  else
    encoder0Pos--;    //encoder is moving backward
  currentAngle = (int) (12 * encoder0Pos / 77 / 10);
}



