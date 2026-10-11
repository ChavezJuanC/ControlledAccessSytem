
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_SS 10

Adafruit_PN532 nfc(PN532_SS);

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando PN532 por SPI...");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();

  if (!versiondata) {
    Serial.println("PN53x no encontrado");
    while (1);
  }

  Serial.print("Encontrado PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);

  Serial.print("Firmware: ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print(".");
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();

  Serial.println("Esperando tarjeta...");
}

void loop() {
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;

  boolean success = nfc.readPassiveTargetID(
    PN532_MIFARE_ISO14443A,
    uid,
    &uidLength
  );

  if (success) {
    Serial.println("Tarjeta encontrada");
    Serial.print("UID: ");

    for (uint8_t i = 0; i < uidLength; i++) {
      if (uid[i] < 0x10) {
        Serial.print("0");
      }

      Serial.print(uid[i], HEX);
      Serial.print(" ");
    }

    Serial.println();

    delay(1000);
  }
}