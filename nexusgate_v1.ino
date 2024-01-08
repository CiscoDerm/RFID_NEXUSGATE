#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include "rgb_lcd.h"

#define SS_PIN 10
#define RST_PIN 9
#define BUTTON_PIN 2

MFRC522 mfrc522(SS_PIN, RST_PIN);
rgb_lcd lcd;

String authorizedUIDs[2] = {"63 CB A4 FC", ""}; // Stockage pour 2 UIDs
bool addMode = false; // Mode d'ajout de carte activé
unsigned long addModeStartTime; // Pour suivre le début du mode d'ajout

void setup() {
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(16, 2);

  pinMode(BUTTON_PIN, INPUT);
  lcd.setRGB(255, 255, 255);
  lcd.print("Pret a scanner");
}

void loop() {
  // Vérifie si une carte est présente
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    if (addMode && millis() - addModeStartTime > 5000) { // Quitter le mode ajout après 5 secondes
      addMode = false;
      lcd.clear();
      lcd.setRGB(255, 255, 255);
      lcd.print("Mode Ajout Termine");
      delay(3000); // Affiche le message pendant 2 secondes
      lcd.clear();
      lcd.setRGB(255, 255, 255);
      lcd.print("Hotel Cyber");
    }
    return;
  }
  
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  lcd.clear();

  String content= "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  // Vérifie si la carte est autorisée
  bool accessGranted = false;
  for (int i = 0; i < 2; i++) {
    if (content.substring(1) == authorizedUIDs[i]) {
      accessGranted = true;
      break;
    }
  }

  if (content.substring(1) == authorizedUIDs[0]) { // Vérifie si c'est la carte administrateur
    lcd.setRGB(0, 255, 0); // Vert
    lcd.print("Admin OK");
    delay(3000);
    lcd.clear();
    lcd.setRGB(255, 255, 255);
    lcd.print("Hotel Cyber");

    // Vérifie si le bouton est pressé pour activer le mode d'ajout
    if (digitalRead(BUTTON_PIN) == HIGH) {
      delay(50); // Debounce du bouton
      if (digitalRead(BUTTON_PIN) == HIGH) {
        addMode = true;
        addModeStartTime = millis(); // Marque le début du mode d'ajout
        lcd.clear();
        lcd.setRGB(255, 165, 0); // Orange
        lcd.print("Mode Ajout Carte");
      }
    }
  } else if (accessGranted) {
    lcd.setRGB(0, 255, 0); // Vert
    lcd.print("Acces OK");
    delay(3000);
    lcd.clear();
    lcd.setRGB(255, 255, 255);
    lcd.print("Hotel Cyber");
  } else if (addMode) {
    // En mode ajout, stocke le nouvel UID et quitte le mode ajout
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
    lcd.setRGB(255, 0, 0); // Rouge
    lcd.print("Acces Refuse");
    delay(3000);
    lcd.clear();
    lcd.setRGB(255, 255, 255);
    lcd.print("Hotel Cyber");
  }

  mfrc522.PICC_HaltA();
  delay(1000); // Délai avant la prochaine lecture
}
