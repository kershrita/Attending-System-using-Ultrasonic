#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Ultrasonic.h>

#define  Lecho  2
#define  Ltrig  3
#define  Recho  4
#define  Rtrig  5
#define BUZZER_PIN 6
#define RIGHT_LED_PIN 7
#define LEFT_LED_PIN 8

const int I2C_ADDR = 0x27;
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

int threshold = 55;
int counter = 0;
int ignoreTime = 5;
int busyChairs = 0;
int freeChairs = 1000;

Ultrasonic RightSensor(Rtrig, Recho);
Ultrasonic LeftSensor(Ltrig, Lecho);

unsigned long lastSensorTime = 0;
int prevCounter = 0;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  pinMode(Rtrig, OUTPUT);
  pinMode(Recho, INPUT);
  pinMode(Ltrig, OUTPUT);
  pinMode(Lecho, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RIGHT_LED_PIN, OUTPUT);
  pinMode(LEFT_LED_PIN, OUTPUT);

  lcd.print("Busy: ");
  lcd.print(busyChairs);
  lcd.setCursor(0, 1);
  lcd.print("Free: ");
  lcd.print(freeChairs);
}

void loop() {
  int Rdistance = RightSensor.read();
  int Ldistance = LeftSensor.read();

  if (Rdistance < threshold && Ldistance >= threshold && millis() - lastSensorTime >= ignoreTime * 1000) {
    counter = max(counter - 1, 0);
    busyChairs = counter;
    freeChairs = 1000 - counter;

    lcd.clear();
    lcd.print("Busy: ");
    lcd.print(busyChairs);
    lcd.setCursor(0, 1);
    lcd.print("Free: ");
    lcd.print(freeChairs);

    digitalWrite(RIGHT_LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(RIGHT_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    lastSensorTime = millis();
  } else if (Ldistance < threshold && Rdistance >= threshold && millis() - lastSensorTime >= ignoreTime * 1000) {
    counter++;
    busyChairs = counter;
    freeChairs = 1000 - counter;

    lcd.clear();
    lcd.print("Busy: ");
    lcd.print(busyChairs);
    lcd.setCursor(0, 1);
    lcd.print("Free: ");
    lcd.print(freeChairs);

    digitalWrite(LEFT_LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(LEFT_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    lastSensorTime = millis();
  }

  if (Rdistance < threshold && Ldistance < threshold) {
    if (prevCounter < counter) {
      Ldistance = threshold + 1;
    } else if (prevCounter > counter) {
      Rdistance = threshold + 1;
    }
  }

  Serial.print("Left sensor distance: ");
  Serial.print(Ldistance);
  Serial.print(" cm, Right sensor distance: ");
  Serial.print(Rdistance);
  Serial.println(" cm");

  prevCounter = counter;
  delay(250);
}