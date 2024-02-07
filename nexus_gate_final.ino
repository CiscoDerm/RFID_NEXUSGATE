#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <Servo.h>

#define SS_PIN 10 // lcd grove
#define RST_PIN 9 // lcd grove
#define BUTTON_PIN 2 // pin ajouter une carte
#define DELETE_BUTTON_PIN 3 // pin pour supprimer la carte
#define SERVO_PIN 8 // pin servo moteur

MFRC522 mfrc522(SS_PIN, RST_PIN);
rgb_lcd lcd;
Servo myServo;

String authorizedUIDs[10] = {"63 CB A4 FC", "", "", "", "", "", "", "", "", ""}; // Stockage pour 10 UIDs
bool addMode = false; // mode pour ajouter la carte
bool deleteMode = false; // mode pour supprimer la carte
unsigned long addModeStartTime;

void setup() {
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(16, 2);
  myServo.attach(SERVO_PIN);

  pinMode(BUTTON_PIN, INPUT); // configuration du bouton pour ajouter la carte
  pinMode(DELETE_BUTTON_PIN, INPUT); // configuration du bouton pour supprimer la carte
  lcd.setRGB(255, 255, 255);
  lcd.print("Pret a scanner");
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
    myServo.write(90); // deplacer le servo
    delay(5000);
    myServo.write(0); // remettre le servo à la position initiale
    delay(3000);
    lcd.clear();
    lcd.setRGB(255, 255, 255);
    lcd.print("Hotel Cyber");

    if (digitalRead(BUTTON_PIN) == HIGH) {
      delay(50);
      if (digitalRead(BUTTON_PIN) == HIGH) {
        addMode = true;
        deleteMode = false; // assure que le mode suppression est desactive
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
        addMode = false; // assure que le mode ajout est desactive
        addModeStartTime = millis();
        lcd.clear();
        lcd.setRGB(255, 0, 0);
        lcd.print("Mode Suppression Carte");
      }
    }
  } else if (accessGranted && !deleteMode) {
    lcd.setRGB(0, 255, 0);
    lcd.print("Acces OK");
    myServo.write(90); // deplacer le servo
    delay(5000);
    myServo.write(0); // remettre le servo à la position initiale
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
        delay(3000);
        lcd.clear();
        lcd.setRGB(255, 255, 255);
        lcd.print("Hotel Cyber");
        break; // sort de la boucle apres avoir ajouté l'UID
      }
    }
  } else if (deleteMode) {
    if (authorizedIndex != -1) {
      authorizedUIDs[authorizedIndex] = ""; // supprime l'UID de la liste autorisee
      lcd.setRGB(255, 0, 0);
      lcd.print("UID Supprime");
      deleteMode = false;
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
