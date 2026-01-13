/* * BOARD: Arduino R4 
 * ROLE: Sensor Hub
 */

#include <Wire.h>
// Include libraries for your specific IMU/Laser if needed (e.g., Adafruit_MPU6050)

// --- PIN DEFINITIONS ---
const int TRIG_F = 4; // Front Ultrasonic
const int ECHO_F = 5;
const int TRIG_R = 6; // Rear Ultrasonic
const int ECHO_R = 7;
const int LASER_PIN = A0; // Assuming Analog Laser sensor
const int SPEED_PIN = 2;  // IR Speed Sensor (Interrupt Pin)

// --- VARIABLES ---
volatile int pulseCount = 0;
unsigned long lastTime = 0;
float currentSpeed = 0;

void countPulse() {
  pulseCount++;
}

long readUltrasonic(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH);
  return duration * 0.034 / 2; // Return cm
}

void setup() {
  Serial1.begin(115200); // UART to ESP32 (Pins 0 & 1)
  
  pinMode(TRIG_F, OUTPUT); pinMode(ECHO_F, INPUT);
  pinMode(TRIG_R, OUTPUT); pinMode(ECHO_R, INPUT);
  pinMode(SPEED_PIN, INPUT_PULLUP);
  
  // Attach interrupt for speed sensor
  attachInterrupt(digitalPinToInterrupt(SPEED_PIN), countPulse, RISING);
}

void loop() {
  // 1. Read Ultrasonics
  long distFront = readUltrasonic(TRIG_F, ECHO_F);
  long distRear = readUltrasonic(TRIG_R, ECHO_R);

  // 2. Read Laser (Example: Analog value map to cm)
  int laserRaw = analogRead(LASER_PIN); 
  int laserDist = map(laserRaw, 0, 1023, 0, 200); // Calibrate this!

  // 3. Calculate Speed (Simple RPM logic)
  if (millis() - lastTime >= 1000) {
    currentSpeed = (pulseCount / 20.0) * 60.0; // Assuming 20 holes in encoder wheel
    pulseCount = 0;
    lastTime = millis();
  }
  
  // 4. Send Packet: "Front,Rear,Laser,Speed"
  // Example: "45,120,88,12.5"
  Serial1.print(distFront); Serial1.print(",");
  Serial1.print(distRear); Serial1.print(",");
  Serial1.print(laserDist); Serial1.print(",");
  Serial1.println(currentSpeed); // Newline ends the packet

  delay(50); // Update 20 times a second
}