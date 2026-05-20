/**
 * Project: Robodog Navigation & AI Bridge
 * Board: ESP32 DevKitV1 (Secondary / "Wake" ESP)
 * Features: Manual Wake Button, Ultrasonic Obstruction Detection, 
 * Professional Buzzer Alerts, and Hardware AI Muting.
 */

// --- Pin Definitions ---
const int BUTTON_PIN = 25;      // Manual wake button
const int XIAOZHI_TRIGGER = 26; // Wire to Xiaozhi GPIO 0 (Wake-up)

const int TRIG_PIN = 4;         // HC-SR04 Trig (Moved from 12 to avoid boot hang)
const int ECHO_PIN = 13;        // HC-SR04 Echo (Requires 1k/2k voltage divider)
const int BUZZER_PIN = 14;      // Piezo/Passive Buzzer output
const int MUTE_PIN = 27;        // Wire to MAX98357A SD pin via Diode

// --- Timing & Logic Constants ---
unsigned long lastWarningTime = 0;
const unsigned long warningInterval = 60000; // 1-minute silence interval
const float thresholdDistance = 20.0;        // Collision limit in cm

void setup() {
  Serial.begin(115200);
  delay(1000); // Wait for Serial to stabilize
  Serial.println("--- Robodog Co-Processor Starting ---");

  // 1. Wake Button & AI Trigger Logic
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(XIAOZHI_TRIGGER, INPUT); // Float by default (Open-drain simulation)

  // 2. Ultrasonic Sensor Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // 3. Buzzer & Mute Control Pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MUTE_PIN, OUTPUT);
  digitalWrite(MUTE_PIN, HIGH); // Default state: Let AI Speak
  
  Serial.println("System Ready. Monitoring Button and Distance...");
}

void loop() {
  checkWakeButton();
  checkObstruction();
}

/**
 * Monitors the physical button. If pressed, it pulls the Xiaozhi's
 * GPIO 0 LOW to trigger the AI interface.
 */
void checkWakeButton() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("Action: Manual Wake-up Triggered.");
    
    pinMode(XIAOZHI_TRIGGER, OUTPUT);
    digitalWrite(XIAOZHI_TRIGGER, LOW);
    
    delay(300); // Simulate a finger press duration
    
    pinMode(XIAOZHI_TRIGGER, INPUT); // Return to high-impedance/float
    delay(500); // Anti-debounce delay
  }
}

/**
 * Checks the distance using the HC-SR04. If an object is too close,
 * it mutes the main AI speaker and plays a professional chirp.
 */
void checkObstruction() {
  // Trigger the ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the duration (30ms timeout to prevent lag)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  float distance = (duration * 0.0343) / 2;

  unsigned long currentTime = millis();

  // Logic for a valid proximity detection
  if (distance > 0 && distance < thresholdDistance) {
    if (currentTime - lastWarningTime >= warningInterval) {
      Serial.print("Alert: Obstruction Detected. distance: ");
      Serial.println(distance);

      // 1. Mute the main AI board audio
      digitalWrite(MUTE_PIN, LOW); 
      
      // 2. LONGER PROFESSIONAL CHIME (Sequence of 6 double-chirps)
      // This increases the duration from ~1 second to ~3 seconds
      for (int i = 0; i < 6; i++) {
        // First Chirp (High)
        tone(BUZZER_PIN, 2600, 70); 
        delay(90);
        
        // Second Chirp (Slightly Lower)
        tone(BUZZER_PIN, 2100, 70); 
        delay(90);
        
        // Small pause between pairs
        // The delay increases slightly each time to create a "slowing" effect
        delay(150 + (i * 20)); 
      }
      
      noTone(BUZZER_PIN);

      // 3. Set timer for the next allowed warning
      lastWarningTime = currentTime;

      // 4. Unmute the main AI board
      digitalWrite(MUTE_PIN, HIGH); 
    }
  }
}