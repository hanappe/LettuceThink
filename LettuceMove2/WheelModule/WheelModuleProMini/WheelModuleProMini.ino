#include <AccelStepper.h>

#define DIRECTION_DIR 5
#define DIRECTION_STEP 6
#define DIRECTION_ENABLE 13
#define encoder0PinA  2
#define encoder0PinB  4

volatile long encoder0Pos = 0;
AccelStepper stepper(AccelStepper::DRIVER,  DIRECTION_STEP,  DIRECTION_DIR);

#define STEPPER_15 1
#define STEPPER_77 0

void setup()
{ 
  Serial.begin(115200);
#if STEPPER_15
  stepper.setMaxSpeed(4000);
  stepper.setAcceleration(2500);
#elif STEPPER_77
  stepper.setMaxSpeed(5000);
  stepper.setAcceleration(3000);
#else 
#error Please define the stepper
#endif

  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);       // turn on pull-up resistor
  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);       // turn on pull-up resistor
  EICRA |= 0x03;  //select rising edge on INT0 (digital 2)
  EIMSK |= 0x01;  //enable INT0 interrupts
}


#if STEPPER_15
static float GEARBOX = (15.0f+3.0f/10.0f);
#elif STEPPER_77
static float GEARBOX = (76.0f+49.0f/64.0f);
#else 
#error Please define the stepper
#endif

static float MICROSTEPS = 2.0f;
static float STEPS_PER_360 = GEARBOX * 200.0f * MICROSTEPS;
static long MAX_STEPS = (long) (1.0f + STEPS_PER_360 / 4.0f); // 90 degrees

int currentAngle = 0;
unsigned long lastRead = 0;


void loop()
{
  unsigned long now = millis();
  if (now - lastRead > 10) {
    lastRead = now;
    int targetAngle = map(analogRead(A0), 0, 1023, -90, 90);
    if (abs(targetAngle - currentAngle) > 1) {
      long current = stepper.currentPosition();
      long delta = map(targetAngle - currentAngle, -90, 90, -MAX_STEPS, MAX_STEPS);
      stepper.moveTo(current + delta);
      Serial.print(targetAngle);
      Serial.print(' ');
      Serial.print(currentAngle);
      Serial.print(' ');
      Serial.print(current);
      Serial.print(' ');
      Serial.println(delta);
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
#if STEPPER_15
  currentAngle = (int) (36 * encoder0Pos / 459); // pos*360/300/15.3
#elif STEPPER_77
  // 1 encoder step corresponds to 360/300 degrees on the stepper axis 
  // 1 degree on the stepper axis corresponds to 1/(76+49/64) degrees on the wheel axis, or 1/76.765625 degrees
  // => 1 encoder step corresponds to 360/300/76.765625 degrees on the wheel axis, or approx. 360/23030 degrees
#else 
#error Please define the stepper
#endif
}



