#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <Servo.h>
#include <EEPROM.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUTTON_PIN 2
#define DELETE_BUTTON_PIN 3
#define SERVO_PIN 8

MFRC522 mfrc522(SS_PIN, RST_PIN);
rgb_lcd lcd;
Servo myServo;

String authorizedUIDs[10]; // Stockage pour 10 UIDs
bool addMode = false;
bool deleteMode = false;
unsigned long addModeStartTime;

void loadAuthorizedUIDs() {
  for (int i = 0; i < 10; i++) {
    // Charger les UID depuis l'EEPROM
    char uidBuffer[9];
    EEPROM.get(i * 9, uidBuffer);
    authorizedUIDs[i] = String(uidBuffer);
  }
}

void saveAuthorizedUIDs() {
  for (int i = 0; i < 10; i++) {
    // Sauvegarder les UID dans l'EEPROM
    char uidBuffer[9];
    authorizedUIDs[i].toCharArray(uidBuffer, 9);
    EEPROM.put(i * 9, uidBuffer);
  }
  EEPROM.commit();
}

void setup() {
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(16, 2);
  myServo.attach(SERVO_PIN);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(DELETE_BUTTON_PIN, INPUT);
  lcd.setRGB(255, 255, 255);
  lcd.print("Pret a scanner");

  loadAuthorizedUIDs();
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    if ((addMode || deleteMode) && millis() - addModeStartTime > 5000) {
      addMode = false;
      deleteMode = false;
      lcd.clear();
      lcd.setRGB(255, 255, 255);
      lcd.print("Mode Termine");
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
  int authorizedIndex = -1;
  for (int i = 0; i < 10; i++) {
    if (content.substring(1) == authorizedUIDs[i]) {
      accessGranted = true;
      authorizedIndex = i;
      break;
    }
  }

  if (content.substring(1) == authorizedUIDs[0]) {
    lcd.setRGB(0, 255, 0);
    lcd.print("Admin OK");
    myServo.write(90); // Déplacer le servo
    delay(3000);
    myServo.write(0); // Remettre le servo à la position initiale
    delay(3000);
    lcd.clear();
    lcd.setRGB(255, 255, 255);
    lcd.print("Hotel Cyber");

    if (digitalRead(BUTTON_PIN) == HIGH) {
      delay(50);
      if (digitalRead(BUTTON_PIN) == HIGH) {
        addMode = true;
        deleteMode = false; // Assure que le mode suppression est désactivé
        addModeStartTime = millis();
        lcd.clear();
        lcd.setRGB(255, 165, 0);
        lcd.print("Mode Ajout Carte");
      }
    }

    if (digitalRead(DELETE_BUTTON_PIN) == HIGH) {
      delay(50);
      if (digitalRead(DELETE_BUTTON_PIN) == HIGH) {
        deleteMode = true;
        addMode = false; // Assure que le mode ajout est désactivé
        addModeStartTime = millis();
        lcd.clear();
        lcd.setRGB(255, 0, 0);
        lcd.print("Mode Suppression Carte");
      }
    }
  } else if (accessGranted && !deleteMode) {
    lcd.setRGB(0, 255, 0);
    lcd.print("Acces OK");
    myServo.write(90); // Déplacer le servo
    delay(3000);
    myServo.write(0); // Remettre le servo à la position initiale
    lcd.clear();
    lcd.setRGB(255, 255, 255);
    lcd.print("Hotel Cyber");
  } else if (addMode) {
    for (int i = 1; i < 10; i++) {
      if (authorizedUIDs[i] == "") {
        authorizedUIDs[i] = content.substring(1);
        lcd.setRGB(0, 255, 0);
        lcd.print("UID Ajoute");
        addMode = false;
        saveAuthorizedUIDs(); // Sauvegarder dans l'EEPROM après ajout
        delay(3000);
        lcd.clear();
        lcd.setRGB(255, 255, 255);
        lcd.print("Hotel Cyber");
        break; // Sort de la boucle après avoir ajouté l'UID
      }
    }
  } else if (deleteMode) {
    if (authorizedIndex != -1) {
      authorizedUIDs[authorizedIndex] = ""; // Supprime l'UID de la liste autorisée
      lcd.setRGB(255, 0, 0);
      lcd.print("UID Supprime");
      deleteMode = false;
      saveAuthorizedUIDs(); // Sauvegarder dans l'EEPROM après suppression
      delay(3000);
      lcd.clear();
      lcd.setRGB(255, 255, 255);
      lcd.print("Hotel Cyber");
    } else {
      lcd.setRGB(255, 0, 0);
      lcd.print("UID Non Trouve");
      delay(3000);
      lcd.clear();
      lcd.setRGB(255, 255, 255);
      lcd.print("Hotel Cyber");
      deleteMode = false;
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
