/*
  Wheel testing — direction, speed, and brake control with encoder-based
  speed measurement (RPM / MPH / KPH) for both drive wheels.

  Turns wheels in forward and reverse direction and applies brake:
  - Enter 'C' on the serial monitor -> wheels move clockwise
  - Enter 'A' on the serial monitor -> wheels move anti-clockwise
  - Enter 'L' on the serial monitor -> turn left
  - Enter 'R' on the serial monitor -> turn right
  - Enter 'B' on the serial monitor -> apply brake

  Hardware: Arduino UNO + JY01-99F2 Motor Driver + Brushless DC hub motors.
  Motor driver JY01-99F2 control port pinout: 5V, EL (0-2.5V/0-5V linear
  brake), M (speed pulse output), ZF (directional control), VR (linear
  speed regulation), GND (control signal ground).

  Wiring:
    Right Motor  -> ZF: pin 2 | M: pin 12 | VR: pin 6 | EL: pin 3 | 5V/GND to Arduino
    Left  Motor  -> ZF: pin 7 | M: pin 10 | VR: pin 5 | EL: pin 9 | 5V/GND to Arduino

  Transcribed from the project's Final Year Thesis, Appendix A.
*/

// Constants
const unsigned long SPEED_TIMEOUT = 500000;   // Time used to determine wheel is not spinning
const unsigned int UPDATE_TIME = 500;         // Time used to output serial data
const unsigned int BUFFER_SIZE = 16;          // Serial receive buffer size
const double BAUD_RATE = 115200;              // Serial port baud rate
const double WHEEL_DIAMETER_IN = 6.5;         // Motor wheel diameter (inches)
const double WHEEL_CIRCUMFERENCE_IN = 22.25;  // Motor wheel circumference (inches)
const double WHEEL_DIAMETER_CM = 16.5;        // Motor wheel diameter (centimeters)
const double WHEEL_CIRCUMFERENCE_CM = 56.5;   // Motor wheel circumference (centimeters)

// Pin Declarations
// RIGHT MOTOR 1
int dir_RA = 2;          // ZF direction
int brake_RA = 3;        // EL brake
int PWM_RA = 6;          // VR PWM control
const int SPEED_RA = 12; // M speed pulse

// LEFT MOTOR 2
int dir_RB = 7;          // ZF direction
int brake_RB = 9;        // EL brake
int PWM_RB = 5;          // VR PWM control
const int SPEED_RB = 10; // M speed pulse

// Variables used in ReadSpeed() for wheel 1
static bool lastState = false;
static unsigned long last_uS;
static unsigned long timeout_uS;
double _freq, _rpm, _mph, _kph;

// Variables used in ReadSpeed() for wheel 2
static bool lastState2 = false;
static unsigned long last_uS2;
static unsigned long timeout_uS2;
double _freq2, _rpm2, _mph2, _kph2;

// This runs only once at startup
void setup()
{
  // Initialize serial port
  Serial.begin(BAUD_RATE);
  Serial.println("---- Program Started ----");

  // MOTOR 1
  pinMode(dir_RA, OUTPUT);
  pinMode(PWM_RA, OUTPUT);
  pinMode(brake_RA, OUTPUT);
  pinMode(SPEED_RA, INPUT);

  // MOTOR 2
  pinMode(dir_RB, OUTPUT);
  pinMode(PWM_RB, OUTPUT);
  pinMode(brake_RB, OUTPUT);
  pinMode(SPEED_RB, INPUT);
}

// This is the main program loop that runs repeatedly
void loop()
{
  if (Serial.available() > 0)
  {
    int inByte = Serial.read();

    switch (inByte) {

      // moving clockwise direction
      case 'C': // Clockwise rotation
        analogWrite(PWM_RA, 255);   // MOTOR 1 - sets speed via PWM
        digitalWrite(dir_RA, LOW);
        analogWrite(PWM_RB, 255);   // MOTOR 2
        digitalWrite(dir_RB, HIGH);
        delay(2000);
        Serial.println("Clockwise rotation");
        Serial.println(" ");
        break;

      // motor stop
      case 'B': // No rotation
        analogWrite(PWM_RA, 0);     // MOTOR 1 - 0 PWM (speed)
        analogWrite(PWM_RB, 0);     // MOTOR 2
        Serial.println("No rotation");
        Serial.println(" ");
        break;

      // moving anti-clockwise direction
      case 'A': // Anti-clockwise rotation
        analogWrite(PWM_RA, 255);   // MOTOR 1 - maximum PWM (speed)
        digitalWrite(dir_RA, HIGH);
        analogWrite(PWM_RB, 255);   // MOTOR 2
        digitalWrite(dir_RB, LOW);
        delay(2000);
        Serial.println("Anti-clockwise rotation");
        Serial.println(" ");
        break;

      // moving in left side direction
      case 'L': // TURN LEFT
        analogWrite(PWM_RA, 255);       // MOTOR 1 - maximum PWM (speed)
        analogWrite(brake_RA, LOW);     // set brake to LOW
        digitalWrite(dir_RA, LOW);
        analogWrite(PWM_RB, 25);        // MOTOR 2
        analogWrite(brake_RB, HIGH);    // set brake to HIGH
        digitalWrite(dir_RB, HIGH);
        delay(2000);
        Serial.println("LEFT SIDE ROTATION");
        Serial.println(" ");
        break;

      // moving in right side direction
      case 'R': // TURN RIGHT
        analogWrite(PWM_RA, 25);        // MOTOR 1
        analogWrite(brake_RA, HIGH);    // set brake to HIGH
        digitalWrite(dir_RA, LOW);
        analogWrite(PWM_RB, 255);       // MOTOR 2 - maximum PWM (speed)
        analogWrite(brake_RB, LOW);     // set brake to LOW
        digitalWrite(dir_RB, HIGH);
        delay(2000);
        Serial.println("RIGHT SIDE ROTATION");
        Serial.println(" ");
        break;

      default:
        // Turn off the motor if any other key is being pressed
        for (int thisPin = 2; thisPin < 11; thisPin++) {
          digitalWrite(thisPin, LOW);
        }
        Serial.println("Wrong key is pressed");
        break;
    }
  }

  // Read the speed from input pin (sets _freq, _rpm, _mph, _kph for both wheels)
  ReadSpeed();

  // Outputs the speed data to the serial port
  WriteToSerial();
}

// Reads the speed from the input pin and calculates RPM and MPH.
// Monitors the state of the input pin and measures the time (uS) between
// pin transitions, for both wheel encoders.
void ReadSpeed()
{
  // ---- Wheel 1 ----
  bool state = digitalRead(SPEED_RA);

  if (state != lastState)
  {
    unsigned long current_uS = micros();
    unsigned long elapsed_uS = current_uS - last_uS;

    double period_uS = elapsed_uS * 2.0;
    _freq = (1 / period_uS) * 1E6;

    _rpm = _freq / 45 * 60;
    if (_rpm > 5000) _rpm = 0;   // ignore excessively high spurious readings

    _mph = (WHEEL_CIRCUMFERENCE_IN * _rpm * 60) / 63360;
    _kph = (WHEEL_CIRCUMFERENCE_CM * _rpm * 60) / 100000;

    last_uS = current_uS;
    timeout_uS = last_uS + SPEED_TIMEOUT;
    lastState = state;
  }
  else if (micros() > timeout_uS)
  {
    // If too long has passed then the wheel has probably stopped
    _freq = 0; _rpm = 0; _mph = 0; _kph = 0;
    last_uS = micros();
  }

  // ---- Wheel 2 ----
  bool state2 = digitalRead(SPEED_RB);

  if (state2 != lastState2)
  {
    unsigned long current_uS2 = micros();
    unsigned long elapsed_uS2 = current_uS2 - last_uS2;

    double period_uS2 = elapsed_uS2 * 2.0;
    _freq2 = (1 / period_uS2) * 1E6;

    _rpm2 = _freq2 / 45 * 60;
    if (_rpm2 > 5000) _rpm2 = 0;

    _mph2 = (WHEEL_CIRCUMFERENCE_IN * _rpm2 * 60) / 63360;
    _kph2 = (WHEEL_CIRCUMFERENCE_CM * _rpm2 * 60) / 100000;

    last_uS2 = current_uS2;
    timeout_uS2 = last_uS2 + SPEED_TIMEOUT;
    lastState2 = state2;
  }
  else if (micros() > timeout_uS2)
  {
    _freq2 = 0; _rpm2 = 0; _mph2 = 0; _kph2 = 0;
    last_uS2 = micros();
  }
}

// Writes the RPM and MPH for both wheels to the serial port at a set interval
void WriteToSerial()
{
  static unsigned long updateTime;
  static unsigned long updateTime2;

  if (millis() > updateTime)
  {
    Serial.print((String)"Freq:" + _freq + " ");
    Serial.print((String)"RPM:" + _rpm + " ");
    Serial.print((String)"MPH:" + _mph + " ");
    Serial.println((String)"KPH:" + _kph + " ");
    updateTime = millis() + UPDATE_TIME;
  }

  if (millis() > updateTime2)
  {
    Serial.print((String)"Freq2:" + _freq2 + " ");
    Serial.print((String)"RPM2:" + _rpm2 + " ");
    Serial.print((String)"MPH2:" + _mph2 + " ");
    Serial.println((String)"KPH2:" + _kph2 + " ");
    updateTime2 = millis() + UPDATE_TIME;
  }
}
