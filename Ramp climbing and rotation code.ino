#include <Wire.h>
#include <LiquidCrystal.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ---------------------
// LCD setup
// ---------------------
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// ---------------------
// Create MPU6050 object
// ---------------------
Adafruit_MPU6050 mpu;

// ---------------------
// Motor pins
// ---------------------
#define ENA 3
#define IN1 2
#define IN2 1
#define ENB 11
#define IN3 12
#define IN4 13

int speedMotor = 200;  // PWM speed (0-255)

// ---------------------
// MPU accelerometer variables (unchanged)
// ---------------------
long accelX, accelY, accelZ;
float angleOffset = 0;
float pitch = 0;
float filterStrength = 0.20;

// ---------------------
// Robot state variables
// ---------------------
bool climbing = false;
bool topReached = false;
float maxClimbAngle = 0;

// ---------------------
// Gyro-based rotation variables
// ---------------------
float yawAngle = 0;
unsigned long lastTime = 0;

// ---------------------
// Read MPU accelerometer (unchanged)
// ---------------------
void readMPU() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 6, true);

  accelX = Wire.read() << 8 | Wire.read();
  accelY = Wire.read() << 8 | Wire.read();
  accelZ = Wire.read() << 8 | Wire.read();
}

// ---------------------
// Compute angle from accelerometer (unchanged)
// ---------------------
float computeRawAngle() {
  float ax = accelX;
  float ay = accelY;
  float az = accelZ;

  float angle = atan2(ay, sqrt(ax * ax + az * az)) * 180 / PI;
  return angle;
}

// ---------------------
// Motor functions
// ---------------------
void forward(int spd) {
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// ---------------------
// Gyro-based 360° turn
// ---------------------
void turn360() {
  yawAngle = 0;
  lastTime = millis();

  // Spin robot in place: left forward, right backward
  analogWrite(ENA, speedMotor);
  analogWrite(ENB, speedMotor);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);

  sensors_event_t a, g, temp;

  while (yawAngle < 360.0) {
    mpu.getEvent(&a, &g, &temp); // read gyro

    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0; // convert ms to seconds
    lastTime = now;

    float rateZ = g.gyro.z * 180.0 / 3.14159; // convert rad/s to deg/s
    yawAngle += abs(rateZ) * dt;              // integrate to get total yaw angle

    lcd.setCursor(0, 0);
    lcd.print("Yaw:");
    lcd.print(yawAngle);
    lcd.print("   ");

    delay(5); // small delay for stability
  }

  stopMotors(); // stop after completing 360°
}

// ---------------------
// Setup
// ---------------------
void setup() {
  Wire.begin();
  Serial.begin(9600);

  lcd.begin(16, 2);

  // ---------------------
  // Initialize MPU6050 gyro
  // ---------------------
  if (!mpu.begin()) {
    lcd.print("MPU fail");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // ---------------------
  // Hardcoded 1-second forward movement at startup
  // ---------------------
  lcd.print("Starting Forward");
  forward(speedMotor);
  delay(1000);
  stopMotors();
  lcd.clear();

  // ---------------------
  // MPU accelerometer calibration (unchanged)
  // ---------------------
  lcd.print("Calibrating...");
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(600);
  readMPU();
  angleOffset = computeRawAngle();
  lcd.clear();
  lcd.print("Offset=");
  lcd.print(angleOffset);
  delay(600);
  lcd.clear();
}

// ---------------------
// Main loop
// ---------------------
void loop() {
  readMPU();

  float raw = computeRawAngle();
  pitch = pitch * (1 - filterStrength) + raw * filterStrength;
  float angle = pitch - angleOffset;

  lcd.setCursor(0, 0);
  lcd.print("Angle:");
  lcd.print(angle);
  lcd.print("   ");

  // ---------------------
  // 1) Flat path
  // ---------------------
  if (!climbing && !topReached && abs(angle) < 3) {
    forward(speedMotor);
    lcd.setCursor(0, 1);
    lcd.print("Flat Path       ");
  }

  // ---------------------
  // 2) Start climbing
  // ---------------------
  if (angle < -7 && !climbing) {
    climbing = true;
    maxClimbAngle = angle;
    lcd.setCursor(0, 1);
    lcd.print("Climbing...     ");
  }

  // Update max angle while climbing
  if (climbing) {
    if (angle < maxClimbAngle) maxClimbAngle = angle;
  }

  // ---------------------
  // 3) Detect top and turn 360 using gyro
  // ---------------------
  if (climbing && !topReached && abs(angle) < 3) {
    stopMotors();
    topReached = true;
    climbing = false;

    // 4-second stop with live angle and max climb display
    unsigned long stopStart = millis();
    while (millis() - stopStart < 4000) {
      readMPU();
      float rawStop = computeRawAngle();
      pitch = pitch * (1 - filterStrength) + rawStop * filterStrength;
      float angleStop = pitch - angleOffset;

      lcd.setCursor(0, 0);
      lcd.print("Angle:");
      lcd.print(angleStop);
      lcd.print("   ");

      lcd.setCursor(0, 1);
      lcd.print("Max:");
      lcd.print(maxClimbAngle);
      lcd.print("       ");

      delay(50);
    }

    lcd.setCursor(0, 1);
    lcd.print("Turning 360...   ");
    turn360(); // use gyro-based 360° turn
  }

  // ---------------------
  // 4) Going down
  // ---------------------
  if (topReached && angle > 5) {
    forward(speedMotor);
    lcd.setCursor(0, 1);
    lcd.print("Going Down...   ");
  }

  delay(20);
}
