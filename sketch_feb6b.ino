#include <Arduino.h>

#define SERVO_PIN 9        // Modifier selon votre branchement
#define PWM_FREQ 50        // Fréquence du signal PWM (50 Hz)
#define PWM_RESOLUTION 10  // Résolution du PWM (10 bits -> 0-1023)

// Fonction pour convertir l'angle en signal PWM
int angleToDuty(int angle) {
  int minDuty = 50;   // Correspond à ~500µs (0°) sur 10 bits
  int maxDuty = 125;  // Correspond à ~2500µs (180°) sur 10 bits
  return map(angle, 0, 180, minDuty, maxDuty);
}

void moveServo(int angle) {
  int dutyCycle = angleToDuty(angle);
  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" -> Duty Cycle: ");
  Serial.println(dutyCycle);
  ledcWrite(SERVO_PIN, dutyCycle);
}

void setup() {
  Serial.begin(115200);

  // Nouvelle API LEDC pour l'ESP32-S3
  ledcAttach(SERVO_PIN, PWM_FREQ, PWM_RESOLUTION);
}

void loop() {
  // Mouvement fluide et continu
  for (int angle = 0; angle <= 180; angle += 5) {
    moveServo(angle);
    delay(30);  // Augmente la vitesse
  }

  for (int angle = 180; angle >= 0; angle -= 5) {
    moveServo(angle);
    delay(30);
  }
}
