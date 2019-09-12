/*
 * GoBabyGo Car Controller
 * Written by FIRST Team 1939 (The Kuhnigits)
 *
 * Modified and simplified by Christopher Scheidel (https://github.com/crscheid)
 * to utilize Cytron PS2 Shield as parent override device.
 *
 * Parent mode is actived by pressing the D-PAD or SQUARE button. Child mode is
 * reactived by pressing the START button.
 */

#include <Servo.h>
#include "Cytron_PS2Shield.h"

// Cytron_PS2Shield setup
//Cytron_PS2Shield ps2(2, 3); // SoftwareSerial: Rx and Tx pin
Cytron_PS2Shield ps2(0,1); // HardwareSerial

// Mechanical Setup
const boolean SPEED_POTENTIOMETER = true;  // Whether or not a speed pot is present
const boolean DISTANCE_WARNING    = false;  // Whether or not distance warning is present
const boolean ROTATE_XY_DIAGONAL  = true;   // Whether or not to rotate child's joystick to diagonal
const boolean DISABLE_BACKWARDS   = true;   // Whether or not to disable backward travel for child

// Invert one or two of the motors - set this per build based on how or the motors were wired
const boolean INVERT_1 = false;
const boolean INVERT_2 = true;

// Constants
const int SPEED_LIMIT = 256;        // Between 0-512
const int SPEED_MINIMUM = 64;       // Between 0-SPEED_LIMIT
const int TURN_SPEED_MINIMUM = 50;  // Between 0-128 - Minimum turn speed
const int REVERSE_SPEED_MINIMUM = 64;
const int DEADBAND = 150;           // Rec'd: 150 - Amount of deadzone in the child controller
const int RAMPING          = 10;    // How quickly the vehicle will ramp up to full speed
const int TURN_FACTOR      = 2;     // Rec'd: 2 - Speed dampening factor for turns
const int REVERSE_FACTOR   = 2;     // Rec'd: 2 - Speed dampening factor for reverse
const int WARNING_DISTANCE = 14;    // Distance in inches to sound piezo
const int REVERSE_PULSE    = 1000;  // Talon SR is 1000
const int FORWARD_PULSE    = 2000;  // Talon SR is 2000
const int NEUTRAL_PULSE    = 1500;  // Talon SR is 1500

// Pins
const int JOYSTICK_X = A2;
const int JOYSTICK_Y = A0;
const int MOTOR_1    = 10;
const int MOTOR_2    = 11;
const int SPEED_POT  = A3;
const int ULTRASONIC = 6;
const int PIEZO      = 9;

// Debug Over Serial - Requires a FTDI cable
const boolean DEBUG = false;


// Values used in loop calculations
int x = 512;
int y = 512;
int limit = SPEED_LIMIT;
boolean parent_mode = true;
int motor1TargetSpeed = NEUTRAL_PULSE;
int motor2TargetSpeed = NEUTRAL_PULSE;
int motor1CurrentSpeed = NEUTRAL_PULSE;
int motor2CurrentSpeed = NEUTRAL_PULSE;


// -----Don't Mess With Anything Past This Line-----

Servo motor1;
Servo motor2;

void setup() {

  /*
   Pinmode sets the mode for digital IO. Here this is specifying that the
   pins identified in the constants should be used as inputs.
   */
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);

  /*
   This is using the Servo.h library to do the same. the .attach function attaches
   the servo to the pins.
   */
  motor1.attach(MOTOR_1);
  motor2.attach(MOTOR_2);

  /*
   If we have a speed pot and distance warning HW, then set the pin mode
   */
  if(SPEED_POTENTIOMETER) pinMode(SPEED_POT, INPUT);
  if(DISTANCE_WARNING){
    pinMode(ULTRASONIC, INPUT);
    pinMode(PIEZO, OUTPUT);
  }

  // Start up the PS2 shield
  ps2.begin(9600); // This baudrate must same with the jumper setting at PS2 shield

  /*
   Set up debugging if the flag is on
   */
  if(DEBUG) Serial.begin(9600);
}

void loop() {

  // If the start button is pressed, take off parent mode
  if (ps2.readButton(PS2_START) == 0) {
    parent_mode = false;
    if (DEBUG) {
      Serial.println("CHILD MODE ACTIVATED");
    }
  }

  // If the stop button is pressed, take off parent mode
  if (ps2.readButton(PS2_SQUARE) == 0 || ps2.readButton(PS2_LEFT) == 0 || ps2.readButton(PS2_RIGHT) == 0 || ps2.readButton(PS2_UP) == 0 || ps2.readButton(PS2_DOWN) == 0) {
    if (DEBUG && !parent_mode) {
      Serial.println("PARENT MODE ACTIVATED");
    }
    parent_mode = true;
  }

  // PARENT MODE CONTROL CODE X/Y position setting
  if (parent_mode) {

    // Read the direction from the d-pad
    if (ps2.readButton(PS2_LEFT) == 0) {
      x = 0;
    }
    else if (ps2.readButton(PS2_RIGHT) == 0) {
      x = 1024;
    }
    else if (ps2.readButton(PS2_UP) == 0) {
      y = 1024;
    }
    else if (ps2.readButton(PS2_DOWN) == 0) {
      y = 0;
    }
    // If no button is being pressed, then we are in neutral
    else {
      x = 512;
      y = 512;
    }
  }
  // CHILD MODE CONTROL CODE X/Y position setting
  else {

    // Read from the joystick - this will read 0 to 1023 over 5-volts
    x = analogRead(JOYSTICK_X);
    y = analogRead(JOYSTICK_Y);

    // Zero values within deadband - zero is actually 512 in this case
    if(abs(512-x)<DEADBAND) x = 512;
    if(abs(512-y)<DEADBAND) y = 512;

    /*
      Map values outside deadband to inside deadband. This is basically removing
      the deadband from the 0 to 1023 mapping and ensuring that the final output
      still maps to 0 to 1023.

      The syntax for map: map(value, fromLow, fromHigh, toLow, toHigh)
     */
    if(x>512) x = map(x, 512+DEADBAND, 1023, 512, 1023);
    else if (x<512) x = map(x, 0, 512-DEADBAND, 0, 512);
    if(y>512) y = map(y, 512+DEADBAND, 1023, 512, 1023);
    else if(y<512) y = map(y, 0, 512-DEADBAND, 0, 512);

    /*
     Translate the XY to diagonal coordinates if needed
     */
    if (ROTATE_XY_DIAGONAL) {
        int new_x = 512 + (0.5 * ( x - y ));
        int new_y = 512 + (0.5 * ( x + y - 1024 ));
        x = new_x;
        y = new_y;
    }

    /*
     If we're disabling backwards motion then set the joystick y to the maximum
     of the actual y value or 512
     */
    if (DISABLE_BACKWARDS) {
      y = max(y,512);
    }

  }

  // Debug input x y after translation / parent mode
  if (DEBUG) {
    Serial.print("x: ");
    Serial.print(x);
    Serial.print("  y: ");
    Serial.print(y);
  }


  /*
   This section updates the speed limit based on the analog pot if activated
   */
  if(SPEED_POTENTIOMETER) {
    limit = map(analogRead(SPEED_POT), 1023, 0, SPEED_MINIMUM, SPEED_LIMIT);

    // print limit if available
    if (DEBUG) {
      Serial.print("  l: ");
      Serial.print(limit);
    }
  }

  // Map speeds to within speed limit - this is binding values around the 512 center
  x = map(x, 0, 1023, 512-limit, 512+limit);
  y = map(y, 0, 1023, 512-limit, 512+limit);

  // Dampen the reverse ...
  if (y < 512) {
    // ... by taking a portion of the distance below 512
    y = 512 - ( (512 - y) / REVERSE_FACTOR);

    // Set the minimum reverse speed
    y = min(y,512-REVERSE_SPEED_MINIMUM);
  }
  


  // Print input x y
  if (DEBUG) {
    Serial.print("xl: ");
    Serial.print(x);
    Serial.print("  yl: ");
    Serial.print(y);
  }

  // Drive wheels based on the x and y coordinates
  driveWheels(x, y);


  // Perform the distance warning if enabled.
  if(DISTANCE_WARNING){
    int inches = pulseIn(ULTRASONIC, HIGH)/144;

    if (DEBUG) {
      Serial.print("  i: ");
      Serial.print(inches);
    }

    if(inches<WARNING_DISTANCE){
      setPiezo(true);
    }
    else{
      setPiezo(false);
    }
  }

  if (DEBUG) {
    Serial.print("  pm: ");
    Serial.print(parent_mode);
    Serial.print("\n");
  }

  delay(20); //Make loop run approximately 50hz
}



/*
 * Function to drive the wheels. In this case the values passed in should
 * already be scaled and limited.
 */
void driveWheels(int x, int y) {

  // If we're not centered
  if (x != 512 || y != 512) {

    // Set target speed for forward and back
    motor1TargetSpeed = map(y, 0, 1023, REVERSE_PULSE, FORWARD_PULSE);
    motor2TargetSpeed = map(y, 0, 1023, REVERSE_PULSE, FORWARD_PULSE);

    // Turn the X into a -250 to +250 value
    int xRotationAdjustment = map(x, 0, 1023, (REVERSE_PULSE-FORWARD_PULSE)/2/TURN_FACTOR, (FORWARD_PULSE-REVERSE_PULSE)/2/TURN_FACTOR);

    if (xRotationAdjustment > 0) {
      xRotationAdjustment = max(xRotationAdjustment, TURN_SPEED_MINIMUM);
    }
    else if (xRotationAdjustment < 0) {
      xRotationAdjustment = min(xRotationAdjustment, -TURN_SPEED_MINIMUM);
    }

    motor1TargetSpeed = motor1TargetSpeed + xRotationAdjustment;
    motor2TargetSpeed = motor2TargetSpeed - xRotationAdjustment;

    // Make sure that the speeds don't exceed the REVERSE_PULSE (neg) or FORWARD_PULSE (pos)
    if (motor1TargetSpeed > NEUTRAL_PULSE) motor1TargetSpeed = min(motor1TargetSpeed,FORWARD_PULSE);
    else motor1TargetSpeed = max(motor1TargetSpeed,REVERSE_PULSE);

    if (motor2TargetSpeed > NEUTRAL_PULSE) motor2TargetSpeed = min(motor2TargetSpeed,FORWARD_PULSE);
    else motor2TargetSpeed = max(motor2TargetSpeed,REVERSE_PULSE);

    // Check for inversions
    if (INVERT_1) motor1TargetSpeed = map(motor1TargetSpeed, REVERSE_PULSE, FORWARD_PULSE, FORWARD_PULSE, REVERSE_PULSE);
    if (INVERT_2) motor2TargetSpeed = map(motor2TargetSpeed, REVERSE_PULSE, FORWARD_PULSE, FORWARD_PULSE, REVERSE_PULSE);


  }
  else {
    motor1TargetSpeed = NEUTRAL_PULSE;
    motor2TargetSpeed = NEUTRAL_PULSE;
  }


  // Ramp the speed to where we need to be
  if (motor1CurrentSpeed != motor1TargetSpeed) {
    if (motor1CurrentSpeed < motor1TargetSpeed) {
      motor1CurrentSpeed = min(motor1TargetSpeed,motor1CurrentSpeed+RAMPING);
    }
    else {
      motor1CurrentSpeed = max(motor1TargetSpeed,motor1CurrentSpeed-RAMPING);
    }
  }

  if (motor2CurrentSpeed != motor2TargetSpeed) {
    if (motor2CurrentSpeed < motor2TargetSpeed) {
      motor2CurrentSpeed = min(motor2TargetSpeed,motor2CurrentSpeed+RAMPING);
    }
    else {
      motor2CurrentSpeed = max(motor2TargetSpeed,motor2CurrentSpeed-RAMPING);
    }
  }


  if (DEBUG) {
    Serial.print("MOTOR LC: ");
    Serial.print(motor1CurrentSpeed);
    Serial.print("  LT: ");
    Serial.print(motor1TargetSpeed);
    Serial.print("  RC: ");
    Serial.print(motor2CurrentSpeed);
    Serial.print("  RT: ");
    Serial.print(motor2TargetSpeed);
    Serial.print("\n");
  }

  motor1.writeMicroseconds(motor1CurrentSpeed);
  motor2.writeMicroseconds(motor2CurrentSpeed);

}



boolean trigger = true;
int count = 0;

void setPiezo(boolean state){
  if(state){
    if(count>=4){
      trigger = !trigger;
      count = 0;
    }else{
      if(trigger) tone(PIEZO, 1300);
      else noTone(PIEZO);
    }
    count++;
  }else{
    trigger = false;
    count = 0;
    noTone(PIEZO);
  }
}
