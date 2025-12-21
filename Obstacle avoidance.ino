
#include <LiquidCrystal.h>

// LCD pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Motor pins
#define ENA 3
#define IN1 2
#define IN2 1

#define ENB 11
#define IN3 12
#define IN4 13

int speedMotor = 180;

// Ultrasonic pins (using analog pins as digital)
#define TRIG A5
#define ECHO A4

long distance;

// ---------------------
// Motors
// ---------------------
void forward(int spd) {
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void backward(int spd) {
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void turnRight(int spd, int timeMs) {
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  delay(timeMs);
  stopMotors();
}

void turnLeft(int spd, int timeMs) {
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  delay(timeMs);
  stopMotors();
}

// ---------------------
// Setup
// ---------------------
void setup() {
  // Motor pins
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  // Ultrasonic pins
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // LCD
  lcd.begin(16, 2);
  lcd.print("Obstacle Avoid");
  delay(1000);
  lcd.clear();
}

// ---------------------
// Read ultrasonic distance
// ---------------------
long readUltrasonic() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  long distance = duration * 0.034 / 2; // cm
  return distance;
}

// ---------------------
// Loop
// ---------------------
void loop() {
  distance = readUltrasonic();

  lcd.setCursor(0,0);
  lcd.print("Dist:");
  lcd.print(distance);
  lcd.print(" cm   ");

  if(distance < 35) { // obstacle detected
    stopMotors();
    lcd.setCursor(0,1);
    lcd.print("Obstacle!      ");
    delay(500);

    backward(speedMotor);
    delay(500);

    turnRight(speedMotor, 700); // turn to avoid
  }
  else {
    forward(speedMotor);
    lcd.setCursor(0,1);
    lcd.print("Moving Forward ");
  }
  delay(50);
}
