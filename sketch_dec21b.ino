#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Motor pins
#define ENA 3
#define IN1 2
#define IN2 1

#define ENB 11
#define IN3 12
#define IN4 13

// Sensors
#define LEFT_SENSOR   A1
#define MID_SENSOR    A3
#define RIGHT_SENSOR  A2

// Motor speeds
int leftBase = 145;
int rightBase = 160;
int turnSpeed = 160;

/*ROTARY ENCODER*/
#define LEFT_ENC   A4
#define RIGHT_ENC  A5

volatile long leftPulses = 0;
volatile long rightPulses = 0;

const int pulsesPerRotation = 20;
const float wheelDiameter = 6.5;   // cm

float distance = 0;
bool stoppedOnce = false;

/*ENCODER COUNT FUNCTION */
void countLeft()  { leftPulses++; }
void countRight() { rightPulses++; }

void setup() {
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);
  pinMode(MID_SENSOR, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  /*LCD & ENCODER INIT (ADD) */
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Distance:");

  pinMode(LEFT_ENC, INPUT);
  pinMode(RIGHT_ENC, INPUT);
}

void Forward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void turnRight(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void turnLeft(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void stop(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void loop() {

  /*ENCODER PULSE DETECTION */
  static int lastLeft = LOW;
  static int lastRight = LOW;

  int currentLeft = digitalRead(LEFT_ENC);
  int currentRight = digitalRead(RIGHT_ENC);

  if (currentLeft == HIGH && lastLeft == LOW) {
    countLeft();
  }
  if (currentRight == HIGH && lastRight == LOW) {
    countRight();
  }

  lastLeft = currentLeft;
  lastRight = currentRight;

  /*DISTANCE CALCULATION */
  float avgPulses = (leftPulses + rightPulses) / 2.0;
  distance = 3.1416 * wheelDiameter * (avgPulses / pulsesPerRotation);

  lcd.setCursor(0, 1);
  lcd.print(distance);
  lcd.print(" cm   ");

  /* STOP ONCE AT 450 CM */
  if (distance >= 450 && !stoppedOnce) {
    stop(turnSpeed);
    delay(2000);          // stop 2 seconds
    stoppedOnce = true;  // ONLY ONCE
  }

  /* LINE FOLLOWING */
  int left = digitalRead(LEFT_SENSOR);
  int mid  = digitalRead(MID_SENSOR);
  int right = digitalRead(RIGHT_SENSOR);

  if ((mid == 1) && (left == 1) && (right == 1)) {
    Forward(turnSpeed);
  }
  else if (left == 0 && mid == 0 && right == 1) {
    turnRight(turnSpeed);
  }
  else if (left == 1 && mid == 0 && right == 0) {
    turnLeft(turnSpeed);
  }
  else if (left == 0 && mid == 1 && right == 0){
    stop(turnSpeed);
    while(mid==1);
  }
}
