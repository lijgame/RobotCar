#include <ST_HW_HC_SR04.h>

#include <IRremote.h>  //including libraries of remote control
#include <L298N.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>  // servo
#include <Wire.h>

// pin to for motors,
#define MotorPWM_L 6
#define MotorPWM_R 5
#define MotorIN1_L 7
#define MotorIN2_L 8
#define MotorIN1_R 2
#define MotorIN2_R 4
unsigned short Speed = 150;
L298N MotorL(MotorPWM_L, MotorIN2_L, MotorIN1_L);
L298N MotorR(MotorPWM_R, MotorIN2_R, MotorIN1_R);

// control cmd
#define CMD_Forward 'U'
#define CMD_Backward 'D'
#define CMD_TurnLeft 'L'
#define CMD_TurnRight 'R'
#define CMD_Stop 'S'
#define CMD_SpeedUp '+'
#define CMD_SpeedDown '-'
#define CMD_ServoLeft '<'
#define CMD_ServoRight '>'
#define CMD_AutoDrive 'A'
#define NO_COMMAND ' '
unsigned char command = NO_COMMAND;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// IR Remote
#define RECV_PIN 12               // pin 12 of IR remoter control receiver
IRrecv irrecv(RECV_PIN);          // defining pin 12 of IR remoter control
decode_results results;           // cache of decode of IR remoter control
#define IR_Go 0x00ff629d          // going forward
#define IR_Back 0x00ffa857        // going backward
#define IR_Left 0x00ff22dd        // turning left
#define IR_Right 0x00ffc23d       // turning right
#define IR_Stop 0x00ff02fd        // stop
#define IR_Servo_L 0x00ff6897     // motor turning left
#define IR_Servo_R 0x00ff9867     // motor turning right
#define IR_Speed_UP 0x00ffb04f    // increasing speed
#define IR_Speed_DOWN 0x00ff30cf  // decreasing speed
#define IR_XunJi_Mode 0x00ff18e7
#define IR_Self_Control 0x00ff7a85  // Auto drive
#define IR_IR_Control 0x00ff10ef
#define IR_Bluetooth_Control 0x00ff38c7
#define IR_ESC 0x00ff52ad  // quitting from remote control

// servo
Servo servo;
short angle = 90;
float distance = 25;

// turn time
#define delay_time 500
#define turnTime 400

//
bool bAutoDrive;

// A1 trigger A0 Echo
ST_HW_HC_SR04 ultrasonicSensor(A1, A0);
// state, 1 go 2 back 3 left 4 right 5 stop
unsigned char CarState = 5;
void show_state(void) {
  lcd.setCursor(0, 0);
  lcd.print("Spd        ");
  lcd.setCursor(4, 0);
  lcd.print(Speed, DEC);
  lcd.setCursor(8, 0);
  lcd.print("A    ");
  lcd.setCursor(10, 0);
  lcd.print(angle, DEC);

  lcd.setCursor(0, 1);
  switch (CarState) {
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

  lcd.setCursor(6, 1);
  lcd.print("Dist      ");
  lcd.setCursor(11, 1);
  lcd.print(distance, 1);
}
void setSpeed(unsigned char Left,
              unsigned char Right)  // function of setting speed
{
  MotorL.setSpeed(Left);
  MotorR.setSpeed(Right);
}
void forward()  //  going forward
{
  CarState = 1;
  MotorL.forward();
  MotorR.forward();
}
void backward()  // back up
{
  CarState = 2;
  MotorL.backward();
  MotorR.backward();
}
void turnL()  // turning left(dual wheel)
{
  CarState = 3;

  setSpeed(250, 250);
  MotorL.backward();
  MotorR.forward();
  setSpeed(Speed, Speed);
}
void turnR()  // turning right(dual wheel)
{
  CarState = 4;
  setSpeed(250, 250);
  MotorL.forward();
  MotorR.backward();
  setSpeed(Speed, Speed);
}
void stop()  // stop
{
  CarState = 5;
  MotorL.stop();
  MotorR.stop();
}

void setup() {
  // init lcd
  lcd.init();
  delay(10);
  lcd.backlight();
  lcd.clear();

  // Serial communication
  Serial.begin(9600);

  // init car state
  setSpeed(Speed, Speed);
  stop();
  lcd.setCursor(0, 0);
  lcd.print("Robot Car");

  // init IR Remote
  irrecv.enableIRIn();

  // set up servo
  servo.attach(9);
  servo.write(angle);
  // time out 23200 23.2 ms for about 4 meters
  // 5800 us about 1 meter
  ultrasonicSensor.setTimeout(23200);

  bAutoDrive = false;
}
void parseCMD() {
  command = NO_COMMAND;
  if (Serial.available()) {
    command = Serial.read();
  }
  {
    if (irrecv.decode(&results)) {
      switch (results.value) {
        case IR_Go:
          command = CMD_Forward;
          break;
        case IR_Back:
          command = CMD_Backward;
          break;
        case IR_Left:
          command = CMD_TurnLeft;
          break;
        case IR_Right:
          command = CMD_TurnRight;
          break;
        case IR_Stop:
          command = CMD_Stop;
          break;
        case IR_Speed_UP:
          command = CMD_SpeedUp;
          break;
        case IR_Speed_DOWN:
          command = CMD_SpeedDown;
          break;
        case IR_Servo_L:
          command = CMD_ServoLeft;
          break;
        case IR_Servo_R:
          command = CMD_ServoRight;
          break;
        case IR_Self_Control:
          command = CMD_AutoDrive;
          break;
        default:
          break;
      }
      irrecv.resume();
    }
  }
  if (command == CMD_AutoDrive)
    bAutoDrive = true;
  else if (command != NO_COMMAND)
    bAutoDrive = false;
}
void getDistance() {
  int t = ultrasonicSensor.getHitTime();
  // speed of sound at 28 degree is about 34.7 cm/ms
  if (t == 0)
    distance = 100;
  else
    distance = t / 29.0f;
}
void getDistanceAt(int angle) {
  servo.write(angle);
  delay(50);
  getDistance();
}

void auto_drive() {
  // setSpeed(200, 200);

  // check front distance
  getDistanceAt(90);

  if (distance < 40) {
    // too close, backward a bit
    if (distance < 20) {
      stop();
      delay(300);
      backward();
      show_state();
      delay(500);
    }
    stop();
    delay(200);
    getDistanceAt(180);  // check left
    delay(delay_time);
    float leftD = distance;
    getDistanceAt(0);  // check right
    delay(delay_time);
    servo.write(90);  // look front
    float rightD = distance;
    if (leftD > rightD)
      turnL();
    else
      turnR();
    show_state();
    delay(turnTime);
    stop();
  } else
    forward();
}

void loop() {
  parseCMD();
  if (bAutoDrive)
    auto_drive();
  else {
    getDistance();
    if (distance > 0 && distance < 25 && CarState == 1) stop();

    switch (command) {
      case CMD_Forward:
        forward();
        break;
      case CMD_Backward:
        backward();
        break;
      case CMD_TurnLeft:
        turnL();
        break;
      case CMD_TurnRight:
        turnR();
        break;
      case CMD_Stop:
        stop();
        break;
      case CMD_SpeedUp:
        Speed += 5;
        if (Speed > 255) Speed = 255;
        setSpeed(Speed, Speed);
        break;
      case CMD_SpeedDown:
        Speed -= 5;
        if (Speed < 5) Speed = 5;
        setSpeed(Speed, Speed);
        break;
      case CMD_ServoLeft:
        ++angle;
        if (angle > 180) angle = 180;
        servo.write(angle);
        break;
      case CMD_ServoRight:
        --angle;
        if (angle < 0) angle = 0;
        servo.write(angle);
        break;
      default:
        break;
    }
  }

  show_state();
  // delay(20);
}