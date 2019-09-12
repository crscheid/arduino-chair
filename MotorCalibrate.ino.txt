#include <Servo.h> 

const int MOTOR_1    = 10;
const int MOTOR_2    = 11;

Servo motor1;
Servo motor2;

void setup() 
{ 
  
  motor1.attach(MOTOR_1);
  motor2.attach(MOTOR_2);

  // Start up the PS2 shield
  Serial.begin(9600);

} 

void loop() {
    
    Serial.println("GET READY IN 10 SECONDS - MOTOR 1 PIN 10");
    delay(7000);
    Serial.println("GET READY IN 3 SECONDS - MOTOR 1 PIN 10");
    delay(3000);
    
    Serial.println("NEUTRAL - 1500 for 3 seconds");
    motor1.writeMicroseconds(1500);   // neutral
    delay(3000);

    Serial.println("REVERSE - 1000 for 3 seconds");
    motor1.writeMicroseconds(1000);   // neutral
    delay(3000);

    Serial.println("NEUTRAL - 1500 for 3 seconds");
    motor1.writeMicroseconds(1500);   // neutral
    delay(3000);

    Serial.println("FORWARD - 2000 for 3 seconds");
    motor1.writeMicroseconds(2000);   // neutral
    delay(3000);

    Serial.println("NEUTRAL - 1500 for 3 seconds");
    motor1.writeMicroseconds(1500);   // neutral
    delay(3000);



    Serial.println("GET READY IN 10 SECONDS - MOTOR 2 PIN 11");
    delay(7000);
    Serial.println("GET READY IN 3 SECONDS - MOTOR 2 PIN 11");
    delay(3000);
    
    Serial.println("NEUTRAL - 1500 for 3 seconds");
    motor2.writeMicroseconds(1500);   // neutral
    delay(3000);

    Serial.println("REVERSE - 1000 for 3 seconds");
    motor2.writeMicroseconds(1000);   // neutral
    delay(3000);

    Serial.println("NEUTRAL - 1500 for 3 seconds");
    motor2.writeMicroseconds(1500);   // neutral
    delay(3000);

    Serial.println("FORWARD - 2000 for 3 seconds");
    motor2.writeMicroseconds(2000);   // neutral
    delay(3000);

    Serial.println("NEUTRAL - 1500 for 3 seconds");
    motor2.writeMicroseconds(1500);   // neutral
    delay(3000);
} 