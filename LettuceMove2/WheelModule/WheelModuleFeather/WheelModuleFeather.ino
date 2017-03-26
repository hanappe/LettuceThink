#include <AccelStepper.h>
#include <SPI.h>
#include <Ethernet2.h>

// Used pins
#define SPEED_ENABLE 2
#define SPEED_PWM 3
#define DIRECTION_PWM 6

byte mac[] = {
  0x98, 0x76, 0xB6, 0x10, 0x57, 0x70
};
IPAddress ip(10, 0, 1, 14);
EthernetServer server(10101);

void setup() {
  Serial.begin(115200);

  setupPwm();
  
  setupWheel();
  enableWheel();
  setupDirection();


  /*TCCR1A = 0;//clear all default presets
  TCCR1B = 0;
  TCCR1C = 0;
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
  OCR1A  = 31250; //31250*.000004 = 125ms
  TIMSK1 |= (1 << OCIE1A);//enable output compare interrupt
  */
  // start the Ethernet connection and the server:
  Serial.println("Ethernet.begin");
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

enum _pstatus {
  _opcode = 0,
  _sign_or_digit = 1,
  _digit = 2
};

int pstate = _opcode;
int opcode = 0;
int sign = 1;
int value = 0;

void loop() 
{
  EthernetClient client = server.available();
  if (client) {
     while (client.available()) {
        char c = client.read();
        //Serial.write(c);
        if (c == 'v') { 
          if (pstate == _opcode) {
            opcode = 'v';
            pstate = _sign_or_digit;
            value = 0;
          } else pstate = _opcode;
        } else if (c == 'd') { 
          if (pstate == _opcode) {
            opcode = 'd';
            pstate = _sign_or_digit;
            value = 0;
          } else pstate = _opcode;
          break;
        } else if (c == '-') {
          if (pstate == _sign_or_digit) {
            sign = -1;
            pstate = _digit;
          } else pstate = _opcode; 
        } else if (c >= '0' && c <= '9') {
          if (pstate == _sign_or_digit) {
            sign = 1;
            value = c - '0';
            pstate = _digit;
          } else if (pstate == _digit) {
            value = value * 10 + (c - '0');
          } else pstate = _opcode; 
        } else if (c == ';') {
          if (pstate == _digit) {
            value = sign * value;
            if (opcode == 'v') 
              gotoSpeed(value);
            else if (opcode == 'd') 
              gotoDirection(value);
            pstate = _opcode;
          } else pstate = _opcode; 
        } else {
          pstate = _opcode;
        }
     }
  }
}


// Direction

void setupDirection()
{
  pinMode(DIRECTION_PWM, OUTPUT); 
}

void gotoDirection(float a)
{
  if (a < -90.0f) a = -90.0f;
  if (a > 90.0f) a = 90.0f;
  int pwm = 128 + (int) (127 * a / 90.0f);
  //analogWrite(DIRECTION_PWM, pwm);
  pwmSet6(pwm);
}


// Wheel/speed

#define PWM_LOW 115
#define PWM_HIGH 175

void setupWheel()
{
  pinMode(SPEED_PWM, OUTPUT); 
  pinMode(SPEED_ENABLE, OUTPUT); 
}

void gotoSpeed(int v)
{
  if (v < 0) v = 0;
  if (v > 100) v = 100;
  int pwm = PWM_LOW + v * (PWM_HIGH - PWM_LOW) / 100;
  analogWrite(SPEED_PWM, pwm);
  //pwmSet13(pwm);
}

void enableWheel()
{
    digitalWrite(SPEED_ENABLE, HIGH);
}

void disableWheel()
{
    digitalWrite(SPEED_ENABLE, LOW);
}



/**********************************************************
   Fast PWM on pins 6, 13 (High Speed TIMER 4)
   
   Do not use analogWrite to pins 6 or 13 if using 
   this functions as they use the same timer.
   
   Those functions will conflict with the 
   MSTIMER2 library.
   Uses 7 PWM frequencies between 2930Hz and 187.5kHz
   
   Timer 4 uses a PLL as input so that its clock frequency
   can be up to 96MHz on standard Arduino Leonardo.
   We limit imput frequency to 48MHz to generate 187.5kHz PWM
   If needed, we can double that up to 375kHz
**********************************************************/

// Frequency modes for TIMER4
#define PWM187k 1   // 187500 Hz
#define PWM94k  2   //  93750 Hz
#define PWM47k  3   //  46875 Hz
#define PWM23k  4   //  23437 Hz
#define PWM12k  5   //  11719 Hz
#define PWM6k   6   //   5859 Hz
#define PWM3k   7   //   2930 Hz

// Direct PWM change variables
#define PWM6        OCR4D
#define PWM13       OCR4A

// Terminal count
#define PWM6_13_MAX OCR4C

void setupPwm()
{
  pwm613configure(PWM187k);
}


// Configure the PWM clock
// The argument is one of the 7 previously defined modes
void pwm613configure(int mode)
{
  // TCCR4A configuration
  TCCR4A=0;

  // TCCR4B configuration
  TCCR4B=mode;

  // TCCR4C configuration
  TCCR4C=0;

  // TCCR4D configuration
  TCCR4D=0;

  // TCCR4D configuration
  TCCR4D=0;

  // PLL Configuration
  // Use 96MHz / 2 = 48MHz
  PLLFRQ=(PLLFRQ&0xCF)|0x30;
  // PLLFRQ=(PLLFRQ&0xCF)|0x10; // Will double all frequencies

  // Terminal count for Timer 4 PWM
  OCR4C=255;
}

// Set PWM to D6 (Timer4 D)
// Argument is PWM between 0 and 255
void pwmSet6(int value)
{
  OCR4D=value;   // Set PWM value
  DDRD|=1<<7;    // Set Output Mode D7
  TCCR4C|=0x09;  // Activate channel D
}

// Set PWM to D13 (Timer4 A)
// Argument is PWM between 0 and 255
void pwmSet13(int value)
{
  OCR4A=value;   // Set PWM value
  DDRC|=1<<7;    // Set Output Mode C7
  TCCR4A=0x82;  // Activate channel A
}


