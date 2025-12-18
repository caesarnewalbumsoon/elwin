#include <LiquidCrystal.h>

// LCD Keypad Shield pin assignment: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Motor pins
#define ENA 3
#define IN1 2
#define IN2 1
#define IN3 12
#define IN4 13
#define ENB 11

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moving Forward");

  // Initialize motor pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Set all motors to move forward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH) ;
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 100); // Full speed
  analogWrite(ENB, 100); // Full speed
}

void loop() {
  // Run for 10 seconds
  for (int i = 0; i <= 10; i++) {
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(i);
    lcd.print(" sec  ");
    delay(1000); // Wait 1 second
  }

  // Stop motors after 10 seconds
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  lcd.setCursor(0, 1);
  lcd.print("Stopped       ");

  while (1); 
}
