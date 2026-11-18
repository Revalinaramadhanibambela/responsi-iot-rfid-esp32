#define BLYNK_TEMPLATE_ID "TMPL6JvqNnfTN"
#define BLYNK_TEMPLATE_NAME "RFID Monitoring"
#define BLYNK_AUTH_TOKEN "qEgfGzKSIk2nk4aH6RzLTl3fBhZ9dbx0"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= WIFI =================
char ssid[] = "Repph_";
char pass[] = "Revasunoo2024";

// ================= RFID =================
#define SS_PIN 5
#define RST_PIN 2

MFRC522 rfid(SS_PIN, RST_PIN);

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= VARIABEL =================
int totalScan = 0;
String lastUID = "";
unsigned long lastReconnect = 0;

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Serial.println("START PROGRAM");

  SPI.begin(18, 19, 23, 5);
  Serial.println("SPI OK");

  rfid.PCD_Init();
  Serial.println("RFID OK");

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  Serial.println("LCD OK");

  lcd.setCursor(0, 0);
  lcd.print("RFID Monitoring");
  lcd.setCursor(0, 1);
  lcd.print("Connecting WiFi");

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  if (Blynk.connected()) {
    Blynk.virtualWrite(V1, "RFID Ready");
    Blynk.virtualWrite(V2, totalScan);
    Blynk.virtualWrite(V3, "Connected");
    Serial.println("Blynk Connected");
  } else {
    Serial.println("Blynk Not Connected");
  }

  delay(2000);

  lcd.clear();
  lcd.print("Tempelkan Kartu");
  lcd.setCursor(0, 1);
  lcd.print("Scan RFID...");

  Serial.println("SETUP SELESAI");
}

// ================= LOOP =================
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
  }

  if (!Blynk.connected() && millis() - lastReconnect > 5000) {
    lastReconnect = millis();
    Blynk.connect(1000);
  }

  Blynk.run();

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  lastUID = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      lastUID += "0";
    }

    lastUID += String(rfid.uid.uidByte[i], HEX);
  }

  lastUID.toUpperCase();
  totalScan++;

  Serial.print("UID: ");
  Serial.println(lastUID);

  Serial.print("Total Scan: ");
  Serial.println(totalScan);

  lcd.clear();
  lcd.print("Kartu Terbaca!");
  lcd.setCursor(0, 1);
  lcd.print(lastUID.substring(0, 16));

  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, lastUID);
    Blynk.virtualWrite(V1, "Kartu Terbaca");
    Blynk.virtualWrite(V2, totalScan);
    Blynk.virtualWrite(V3, "Connected");
  }

  delay(1500);

  lcd.clear();
  lcd.print("Tempelkan Kartu");
  lcd.setCursor(0, 1);
  lcd.print("Scan RFID...");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}