// 3 SENSOR LINE FOLLOWING ROBOT
// Middle sensor: 1 = black, 0 = white
// Left & Right sensors: 0 = black, 1 = white (INVERTED)
// Right motor intentionally weaker

// Motor pins
#define ENA 3   // Left motor PWM
#define IN1 2
#define IN2 1

#define ENB 11  // Right motor PWM
#define IN3 12
#define IN4 13

// Sensors
#define LEFT_SENSOR   A1   // inverted
#define MID_SENSOR    A3   // normal
#define RIGHT_SENSOR  A2   // inverted

// Motor speeds
int leftBase = 145;    
int rightBase = 160;
int turnSpeed = 160;

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

  int left = digitalRead(LEFT_SENSOR);      // 1=white, 0=black (inverted)
  int mid  = digitalRead(MID_SENSOR);       // 1=black, 0=white
  int right = digitalRead(RIGHT_SENSOR);    // 1=white, 0=black (inverted)

  // -------------------------`
  // MAIN LOGIC
  // -------------------------

  // Middle sees black → go straight
  if ((mid == 1) && (left == 1) && (right == 1)) {
    Forward(turnSpeed);
  }

  // Left sensor sees black (0) → line is on LEFT → turn RIGHT
  else if (left == 0 && mid == 0 && right == 1) {
    turnRight(turnSpeed);

  }

  // Right sensor sees black (0) → line is on RIGHT → turn LEFT
  else if (left == 1 && mid == 0 && right == 0) {
    turnLeft(turnSpeed);
  }

  // All white: lost → small search
  else if (left == 0 && mid == 1 && right == 0){
    stop(turnSpeed);
    while(mid==1);
  }
}
