#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// I2C address 0x27 or 0x3F, 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);
// -------- ADXL345 Address --------
#define ADXL345_ADDR 0x53
#define DHTPIN PB1  // DHT11 data pin
#define DHTTYPE DHT11
// -------- Sensor Pins --------
#define LDR_PIN PA0
#define TRIG_PIN PB5
#define ECHO_PIN PB3
DHT dht(DHTPIN, DHTTYPE);
long duration;
int distance;
// -------- ADXL345 Functions --------
void adxlWrite(byte reg, byte value) {
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void adxlReadXYZ(int16_t *x, int16_t *y, int16_t *z) {
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(0x32);  // Data register start
  Wire.endTransmission(false);

  Wire.requestFrom(ADXL345_ADDR, 6);
  if (Wire.available() == 6) {
    // Read into a buffer first: in "Wire.read() | (Wire.read() << 8)"
    // C++ does not guarantee which read() runs first, so bytes could swap
    uint8_t buf[6];
    for (int i = 0; i < 6; i++) buf[i] = Wire.read();
    *x = (int16_t)(buf[0] | (buf[1] << 8));
    *y = (int16_t)(buf[2] | (buf[3] << 8));
    *z = (int16_t)(buf[4] | (buf[5] << 8));
  }
}


void setup() {
  Serial.begin(9600);

  lcd.begin();        // Initialize LCD
  lcd.backlight(); 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vehicle to");
  lcd.setCursor(0, 1);
  lcd.print("vehicle comms");
  delay(1500);
  dht.begin();
  // I2C init
  Wire.begin();

  // ADXL345 init
  adxlWrite(0x2D, 0x08);  // Power on, measure mode
  adxlWrite(0x31, 0x08);  // Full resolution, ±2g
  adxlWrite(0x2C, 0x0A);  // 100 Hz data rate

  // Ultrasonic pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // ---- LDR ----
  int ldrValue = analogRead(LDR_PIN);

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Send a 10 microsecond HIGH pulse to trigger
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo time (30 ms timeout, about 5 m range)
  duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // Calculate distance (speed of sound is ~343 m/s).
  // No echo returns 0, which must not be read as "0 cm, vehicle ahead"
  distance = (duration > 0) ? duration * 0.034 / 2 : -1;
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  // ---- ADXL345 ----
  int16_t ax = 0, ay = 0, az = 0;
  adxlReadXYZ(&ax, &ay, &az);
  float accelX = ax * 0.0039;  // scale factor ~3.9 mg/LSB in ±2g mode
  float accelY = ay * 0.0039;
  float accelZ = az * 0.0039;

  // // ---- Serial Debug ----
  // Serial.print("LDR: ");
  // Serial.print(ldrValue);
  // Serial.print(" | Dist: ");
  // Serial.print(distance);
  // Serial.print(" cm");
  // Serial.print(" | X: ");
  // Serial.print(accelX, 2);
  // Serial.print(" Y: ");
  // Serial.print(accelY, 2);


  // ---- LCD Display ----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LDR:");
  lcd.print(ldrValue);
  lcd.setCursor(0, 1);
  lcd.print("Dist:");
  lcd.print(distance);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print("Hum:");
  lcd.print(humidity);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("X:");
  lcd.print(accelX, 2);
  lcd.setCursor(0, 1);
  lcd.print("Y:");
  lcd.print(accelY, 2);
  delay(1000);
  if (distance > 0 && distance < 10) {
    Serial.println("1");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Distance less");
    lcd.setCursor(0, 1);
    lcd.print("Vehicle in front");
    delay(1000);
  }
  if (abs(accelX) > 0.5 || abs(accelY) > 0.5) {
    Serial.println("2");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Accident");
    lcd.setCursor(0, 1);
    lcd.print("detected");
    delay(1000);
  }
  if (ldrValue > 600) {
    Serial.println("3");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Intensity low");
    delay(1000);
  }
  if (temperature > 34) {
    Serial.println("4");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("High temp");
    lcd.setCursor(0, 1);
    lcd.print("detected");
    delay(1000);
  }
}
