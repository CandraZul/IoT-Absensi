#include <Adafruit_Fingerprint.h>

// Gunakan Serial2 (atau sesuaikan dengan pin RX/TX yang Anda gunakan)
HardwareSerial mySerial(2); // UART2 untuk ESP32

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(115200); // Serial Monitor
  mySerial.begin(57600, SERIAL_8N1, 22, 27); // RX=16, TX=17 (sesuaikan dengan pin Anda)

  Serial.println("\nAdafruit Fingerprint sensor test");

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
}

void loop() {
  getFingerprintID();
  delay(50);
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER) {
    return p;
  }

  if (p == FINGERPRINT_OK) {
    Serial.println("Image taken");
  } else {
    Serial.println("Error taking image");
    return p;
  }

  p = finger.image2Tz();
  if (p == FINGERPRINT_OK) {
    Serial.println("Image converted");
  } else {
    Serial.println("Error converting image");
    return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence ");
    Serial.println(finger.confidence);
  } else {
    Serial.println("No match found");
  }

  return p;
}
