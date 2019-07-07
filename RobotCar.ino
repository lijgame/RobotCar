#include <ST_HW_HC_SR04.h>

#include <L298N.h>
#include <LiquidCrystal_I2C.h>
#include <PS2X_lib.h>  //for v1.6
#include <Servo.h>     // servo
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
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// PS2 controller
#define pressures true
#define rumble true
PS2X ps2x;          // create PS2 Controller Class
#define PS2_DAT A3  // 14
#define PS2_CMD A2  // 15
#define PS2_SEL A4  // 16
#define PS2_CLK A1  // 17
int error = 0;
byte type = 0;
byte vibrate = 0;

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
// ST_HW_HC_SR04 ultrasonicSensor(40, 40);
// state, 1 go 2 back 3 left 4 right 5 stop
unsigned char CarState = 5;
void show_sstates(void) {
  //  Serial.print("speed: ");
  //  Serial.print(Speed,DEC);
  //  Serial.print(" Angle: ");
  //  Serial.print(angle,DEC);
  switch (CarState) {
    case 1:
      Serial.println(" Go  ");
      break;
    case 2:
      Serial.println("Back ");
      break;
    case 3:
      Serial.println("Left ");
      break;
    case 4:
      Serial.println("Right");
      break;
    case 5:
      Serial.println("Stop ");
      break;
    default:
      break;
  }
}
/*
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
*/
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
  /*
   // init lcd
   lcd.init();
   delay(10);
   lcd.backlight();
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Robot Car");
 */

  // Serial communication
  Serial.begin(57600);

  // init car state
  setSpeed(Speed, Speed);
  stop();

  //  // set up servo
  //  servo.attach(9);
  //  servo.write(angle);
  //  // time out 23200 23.2 ms for about 4 meters
  //  // 5800 us about 1 meter
  //  ultrasonicSensor.setTimeout(23200);

  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures,
                              rumble);
  if (error == 0) {
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
    if (pressures)
      Serial.println("true ");
    else
      Serial.println("false");
    Serial.print("rumble = ");
    if (rumble)
      Serial.println("true)");
    else
      Serial.println("false");
    Serial.println(
        "Try out all the buttons, X will vibrate the controller, faster as you "
        "press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println(
        "Note: Go to www.billporter.info for updates and to report bugs.");
  } else if (error == 1)
    Serial.println(
        "No controller found, check wiring, see readme.txt to enable debug. "
        "visit www.billporter.info for troubleshooting tips");

  else if (error == 2)
    Serial.println(
        "Controller found but not accepting commands. see readme.txt to enable "
        "debug. Visit www.billporter.info for troubleshooting tips");

  else if (error == 3)
    Serial.println(
        "Controller refusing to enter Pressures mode, may not support it. ");

  //  Serial.print(ps2x.Analog(1), HEX);

  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
    case 2:
      Serial.print("GuitarHero Controller found ");
      break;
    case 3:
      Serial.print("Wireless Sony DualShock Controller found ");
      break;
  }
}
void PS2CMD() {
  /* You must Read Gamepad to get new values and set vibration values
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     if you don't enable the rumble, use ps2x.read_gamepad(); with no values
     You should call this at least once a second
   */
  if (error == 1)  // skip loop if no controller found
    return;

  // DualShock Controller
  ps2x.read_gamepad(false, vibrate);  // read controller and set large motor
                                      // to spin at 'vibrate' speed

  if (ps2x.Button(PSB_START))  // will be TRUE as long as button is pressed
    Serial.println("Start is being held");
  if (ps2x.Button(PSB_SELECT)) Serial.println("Select is being held");

  // will be TRUE as long as button is pressed
  bool bstop = true;
  if (ps2x.Button(PSB_PAD_UP)) {
    Serial.print("Up held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
    forward();
    bstop = false;
  }
  if (ps2x.Button(PSB_PAD_RIGHT)) {
    Serial.print("Right held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
    turnR();
    bstop = false;
  }
  if (ps2x.Button(PSB_PAD_LEFT)) {
    Serial.print("LEFT held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
    turnL();
    bstop = false;
  }
  if (ps2x.Button(PSB_PAD_DOWN)) {
    Serial.print("DOWN held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
    backward();
    bstop = false;
  }

  if (bstop) stop();

  // print stick values if either is TRUE
  if (ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) {
    Serial.print("Stick Values:");
    // Left stick, Y axis. Other options: LX, RY, RX
    Serial.print(ps2x.Analog(PSS_LY), DEC);
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_LX), DEC);
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_RY), DEC);
    Serial.print(",");
    Serial.println(ps2x.Analog(PSS_RX), DEC);
  }
}

void loop() {
  PS2CMD();
  delay(20);
}
