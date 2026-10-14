#include <LiquidCrystal_I2C.h>

#define I2C_ADDR 0x27  // Common address (use 0x3F if your module is different)
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

#define BUZZER_PIN PA1  // choose any free pin
unsigned long lastDataTime = 0;  // to track last received time
const unsigned long timeout = 3000;  // 3 sec timeout for "Everything is OK"

void setup() {
  Serial.begin(9600);  // UART connected to Arduino

  lcd.begin();      // initialize LCD
  lcd.backlight();  // turn on backlight
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STM32 Receiver");
  delay(1000);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    lastDataTime = millis();  // update last received time

    if (c == '1') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vehicle in front");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(2000);
    } 
    else if (c == '2') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Accident Alert");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(2000);
    } 
    else if (c == '3') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Low Light");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(2000);
    } 
    else if (c == '4') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp High");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(2000);
    }

    // turn buzzer OFF after alert
    digitalWrite(BUZZER_PIN, LOW);
  }

  // If no data received for some time → show "Everything is OK"
  if (millis() - lastDataTime > timeout) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Everything is");
    lcd.setCursor(0, 1);
    lcd.print("  OK  ");
    digitalWrite(BUZZER_PIN, LOW);
    lastDataTime = millis();  // refresh to avoid continuous clearing
  }
}