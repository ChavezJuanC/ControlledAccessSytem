#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_SS 10
// RFID
Adafruit_PN532 nfc(PN532_SS);
bool cardFound = false;
//UID: E1 2F 50 68 
const uint8_t validUID[4] = { 0xE1, 0x2F, 0x50, 0x68 };  // Ejemplo de UID valido

// LiquidCrystal setup (Pins 2-7) -> Leaves 10, 11, 12, 13 FREE for RFID
const int rs = 6, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Numpad setup (Using Analog pins + digital 0 and 8)
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { A0, A1, A2, A3 };
byte colPins[COLS] = { 0, 8, A4, A5 };  // Pin 0 and 8 used, 10-13 kept free!

Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Servo motor on Pin 9 (Free)
Servo door_servo;
const int servoPin = 9;

// Inputs
char pinArray[5];
byte pinCount = 0;
const char setPin[5] = "0420";

void setup() {
  lcd.begin(16, 2);
  door_servo.attach(servoPin);
  door_servo.write(0);

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("PN53x not found");
    while (1)
      ;
  }
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
}


//Pin logic
bool validatePin() {
  lcd.print("Enter pin:");
  do {
    char pinInput = customKeypad.getKey();
    if (pinInput) {
      pinArray[pinCount] = pinInput;
      lcd.print('*');
      pinCount++;
    }
  } while (pinCount < 4);

  pinArray[4] = '\0';

  lcd.clear();
  lcd.setCursor(0, 0);

  if (memcmp(pinArray, setPin, 4) == 0) {
    lcd.print("Access Granted");
    resetPinInput();
    return true;
  } else {
    lcd.print("Access Denied");
    resetPinInput();
    return false;
  }
}

void resetPinInput() {
  pinCount = 0;
  delay(1500);
  lcd.clear();
}

bool validateCard() {

  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  do {
    bool success = nfc.readPassiveTargetID(
      PN532_MIFARE_ISO14443A,
      uid,
      &uidLength);

    if (success) {
      lcd.clear();
      lcd.print("Validating...");
      delay(2000);
      Serial.println("Tarjeta encontrada");
      Serial.print("UID: ");

      for (uint8_t i = 0; i < uidLength; i++) {
        if (uid[i] < 0x10) {
          Serial.print("0");
        }

        Serial.print(uid[i], HEX);
        Serial.print(" ");
      }
      cardFound = true;
    }

    lcd.clear();
    delay(1000);

  } while (!cardFound);

  if (isUIDEqual(uid, validUID, uidLength)) {
    return true;
  } else {
    return false;
  }

  lcd.clear();
}


bool isUIDEqual(uint8_t* arrayA, uint8_t* arrayB, uint8_t length) {
  for (uint8_t index = 0; index < length; index++) {
    if (arrayA[index] != arrayB[index]) return false;
  }
  return true;
}

void openDoor() {
  lcd.print("Opening door");
  for (int angle = 0; angle <= 90; angle += 1) {
    door_servo.write(angle);
    delay(15);
  }
  lcd.clear();
}

void closeDoor() {
  lcd.print("Closing door");
  for (int angle = 90; angle > 0; angle -= 1) {
    door_servo.write(angle);
    delay(15);
  }
  lcd.clear();
}

void cycleDoor() {
  openDoor();
  delay(3000);
  closeDoor();
}

void loop() {
  lcd.print("Welcome!");
  if (validateCard() && validatePin()) {
    cycleDoor();
  } else {
    lcd.print("Please Try again");
    delay(2500);
    lcd.clear();
  }
}