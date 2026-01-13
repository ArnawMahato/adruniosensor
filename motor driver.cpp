/* * BOARD: ESP32 (Top Layer)
 * ROLE: Main Controller & Motor Driver
 */

// --- PINS ---
// Motor Driver Pins (e.g., L298N)
const int ENA = 13; // Speed PWM (Left)
const int IN1 = 12;
const int IN2 = 14;
const int IN3 = 27;
const int IN4 = 26;
const int ENB = 25; // Speed PWM (Right)

// UART Pins
#define RXD2 16
#define TXD2 17

void setup() {
  Serial.begin(115200); // Debug to PC
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // From R4
  
  // Setup Motor Pins
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  
  // Configure PWM (LEDC) for ESP32
  // (Standard ESP32 PWM setup code omitted for brevity, use analogWrite if using Arduino 3.0+)
}

void driveMotors(int leftSpeed, int rightSpeed) {
  // Simple driver logic
  // Speed: 0 to 255
  // If speed > 0 move forward, if < 0 move backward
  
  // LEFT MOTOR
  if (leftSpeed > 0) { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); }
  else { digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); leftSpeed = -leftSpeed; }
  analogWrite(ENA, leftSpeed);

  // RIGHT MOTOR
  if (rightSpeed > 0) { digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); }
  else { digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); rightSpeed = -rightSpeed; }
  analogWrite(ENB, rightSpeed);
}

void loop() {
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    
    // Parse the CSV: "Front,Rear,Laser,Speed"
    int idx1 = data.indexOf(',');
    int idx2 = data.indexOf(',', idx1 + 1);
    int idx3 = data.indexOf(',', idx2 + 1);
    
    if (idx3 > 0) {
      int frontDist = data.substring(0, idx1).toInt();
      int rearDist = data.substring(idx1 + 1, idx2).toInt();
      int laserDist = data.substring(idx2 + 1, idx3).toInt();
      float speed = data.substring(idx3 + 1).toFloat();

      // --- DECISION LOGIC ---
      
      // 1. Safety Stop
      if (frontDist < 20 && frontDist > 0) {
        Serial.println("OBSTACLE AHEAD! STOPPING.");
        driveMotors(0, 0);
      }
      // 2. Normal Operation
      else {
        // Example: Cruise Control
        Serial.printf("Cruising. Speed: %.2f Front: %d\n", speed, frontDist);
        driveMotors(150, 150); 
      }
    }
  }
}