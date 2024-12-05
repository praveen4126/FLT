#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// Define the SoftwareSerial pins for communication with the fingerprint sensor
SoftwareSerial mySerial(2, 3);

// Create a fingerprint object
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Relay control pin
#define RELAY_PIN 12

// Variable to store the relay's current state
bool relayState = false; // false = OFF, true = ON

void setup() {
  Serial.begin(9600); // Start Serial Monitor
  while (!Serial);    // Wait for Serial Monitor
  delay(100);
  
  Serial.println("Fingerprint Relay Toggle System");

  // Initialize relay pin as output and set it to LOW (OFF)
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Initialize the fingerprint sensor
  finger.begin(57600);
  
  // Check if the fingerprint sensor is detected
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor detected!");
  } else {
    Serial.println("Fingerprint sensor not detected. Please check connections.");
    while (1) {
      delay(1);
    }
  }

  // Display the number of stored templates
  finger.getTemplateCount();
  Serial.print("Sensor contains ");
  Serial.print(finger.templateCount);
  Serial.println(" enrolled fingerprints");
  Serial.println("Waiting for an authorized fingerprint...");
}

void loop() {
  // Check for a valid fingerprint
  int result = getFingerprintID();
  
  if (result >= 0) { // Fingerprint found
    Serial.println("Authorized fingerprint detected.");

    // Toggle relay state
    relayState = !relayState;
    if (relayState) {
      Serial.println("Relay turned ON.");
      digitalWrite(RELAY_PIN, HIGH); // Turn relay ON
    } else {
      Serial.println("Relay turned OFF.");
      digitalWrite(RELAY_PIN, LOW);  // Turn relay OFF
    }
    
    // Add a small delay to prevent rapid toggling
    delay(500);
  }
}

// Function to get the fingerprint ID
int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1; // No finger detected
  
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1; // Error converting image to template
  
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1; // Fingerprint not found
  
  // Match found
  Serial.print("Fingerprint matched with ID: ");
  Serial.println(finger.fingerID);
  return finger.fingerID; // Return matched fingerprint ID
}
