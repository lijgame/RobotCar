#include <IRremote.h> //including libraries of remote control
#include <L298N.h>
#include <LiquidCrystal_I2C.h>
// #include <Servo.h> // servo
#include <Wire.h>

#define MotorPWM_L 10      // pin of controlling speed---- ENA of motor driver board
#define MotorPWM_R 5       // pin of controlling speed---- ENB of motor driver board
#define MotorIN1_L 7       // pin of controlling turning---- IN1 of motor driver board
#define MotorIN2_L 8       // pin of controlling turning---- IN2 of motor driver board
#define MotorIN1_R 2       // pin of controlling turning---- IN3 of motor driver board
#define MotorIN2_R 4       // pin of controlling turning---- IN4 of motor driver board
unsigned char Speed = 250; // initialized left wheel speed at 200
L298N MotorL(MotorPWM_L, MotorIN2_L, MotorIN1_L);
L298N MotorR(MotorPWM_R, MotorIN2_R, MotorIN1_R);

// control cmd
unsigned char command;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// IR Remote
#define RECV_PIN 12              // pin 12 of IR remoter control receiver
IRrecv irrecv(RECV_PIN);         // defining pin 12 of IR remoter control
decode_results results;          // cache of decode of IR remoter control
#define IR_Go 0x00ff629d         // going forward
#define IR_Back 0x00ffa857       // going backward
#define IR_Left 0x00ff22dd       // turning left
#define IR_Right 0x00ffc23d      // turning right
#define IR_Stop 0x00ff02fd       // stop
#define IR_Servo_L 0x00ff6897    // motor turning left
#define IR_Servo_R 0x00ff9867    // motor turning right
#define IR_Speed_UP 0x00ffb04f   // increasing speed
#define IR_Speed_DOWN 0x00ff30cf // decreasing speed
#define IR_XunJi_Mode 0x00ff18e7
#define IR_Self_Control 0x00ff7a85 // ultrasonic distance detecting
#define IR_IR_Control 0x00ff10ef
#define IR_Bluetooth_Control 0x00ff38c7
#define IR_ESC 0x00ff52ad // quitting from remote control

// state, 1 go 2 back 3 left 4 right 5 stop
unsigned char CarState = 5;
void show_state(void)
{
  lcd.setCursor(0, 1);
  switch (CarState)
  {
  case 1:
    lcd.print(" Go  ");
    break;
  case 2:
    lcd.print("Back ");
    break;
  case 3:
    lcd.print("Left ");
    break;
  case 4:
    lcd.print("Right");
    break;
  case 5:
    lcd.print("Stop ");
    break;
  default:
    break;
  }
}
void Set_Speed(unsigned char Left,
               unsigned char Right) // function of setting speed
{
  MotorL.setSpeed(Left);
  MotorR.setSpeed(Right);
}
void forward() //  going forward
{
  CarState = 1;
  MotorL.forward();
  MotorR.forward();
}
void backward() // back up
{
  CarState = 2;
  MotorL.backward();
  MotorR.backward();
}
void turnL() // turning left(dual wheel)
{
  CarState = 3;
  // MotorL.backward();
  MotorR.forward();
}
void turnR() // turning right(dual wheel)
{
  CarState = 4;
  MotorL.forward();
  // MotorR.backward();
}
void stop() // stop
{
  CarState = 5;
  MotorL.stop();
  MotorR.stop();
}

void setup()
{
  // init lcd
  lcd.init();      // invoking initialized function of LCD in LiquidCrystal_I2C.h
  delay(10);       // delaying for 10 millisecond
  lcd.backlight(); // open backlight of LCD1602
  lcd.clear();     // clear screen

  // Serial communication
  Serial.begin(9600);

  // init car state
  Set_Speed(Speed, Speed); // setting initialized speed
  stop();
  lcd.setCursor(0, 0);
  lcd.print("Wait command");
}
void loop()
{
  if (Serial.available())
  {
    command = Serial.read();
    switch (command)
    {
    case 'U':
      forward();
      break;
    case 'D':
      backward();
      break;
    case 'L':
      turnL();
      break;
    case 'R':
      turnR();
      break;
    case 'S':
    default:
      stop();
      break;
    }
    show_state();
  }
}