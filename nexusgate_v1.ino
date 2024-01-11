#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUTTON_PIN 2
#define SERVO_PIN 8

MFRC522 mfrc522(SS_PIN, RST_PIN);
rgb_lcd lcd;
Servo myServo;

String authorizedUIDs[2] = {"63 CB A4 FC", ""}; // Stockage pour 2 UIDs
bool addMode = false;
unsigned long addModeStartTime;

void setup() {
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(16, 2);
  myServo.attach(SERVO_PIN);

  pinMode(BUTTON_PIN, INPUT);
  lcd.setRGB(255, 255, 255);
  lcd.print("Pret a scanner");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    if (addMode && millis() - addModeStartTime > 5000) {
      addMode = false;
      lcd.clear();
      lcd.setRGB(255, 255, 255);
      lcd.print("Mode Ajout Termine");
      delay(3000);
      lcd.clear();
      lcd.setRGB(255, 255, 255);
      lcd.print("Hotel Cyber");
    }
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  lcd.clear();

  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  bool accessGranted = false;
  for (int i = 0; i < 2; i++) {
    if (content.substring(1) == authorizedUIDs[i]) {
      accessGranted = true;
      break;
    }
  }

  if (content.substring(1) == authorizedUIDs[0]) {
    lcd.setRGB(0, 255, 0);
    lcd.print("Admin OK");
    delay(3000);
    lcd.clear();
    lcd.setRGB(255, 255, 255);
    lcd.print("Hotel Cyber");

    if (digitalRead(BUTTON_PIN) == HIGH) {
      delay(50);
      if (digitalRead(BUTTON_PIN) == HIGH) {
        addMode = true;
        addModeStartTime = millis();
        lcd.clear();
        lcd.setRGB(255, 165, 0);
        lcd.print("Mode Ajout Carte");
      }
    }
  } else if (accessGranted) {
    lcd.setRGB(0, 255, 0);
    lcd.print("Acces OK");
    myServo.write(90); // Déplacer le servo
    delay(3000);
    myServo.write(0); // Remettre le servo à la position initiale
    lcd.clear();
    lcd.setRGB(255, 255, 255);
    lcd.print("Hotel Cyber");
  } else if (addMode) {
    if (authorizedUIDs[1] == "") {
      authorizedUIDs[1] = content.substring(1);
      lcd.setRGB(0, 255, 0);
      lcd.print("UID Ajoute");
      addMode = false;
      delay(3000);
      lcd.clear();
      lcd.setRGB(255, 255, 255);
      lcd.print("Hotel Cyber");
    }
  } else {
    lcd.setRGB(255, 0, 0);
    lcd.print("Acces Refuse");
    delay(3000);
    lcd.clear();
    lcd.setRGB(255, 255, 255);
    lcd.print("Hotel Cyber");
  }

  mfrc522.PICC_HaltA();
  delay(1000);
}
