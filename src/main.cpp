#include <Arduino.h>
#include "Detector.h"

const int TRIG_PIN = 5;
const int ECHO_PIN = 18;

Detector detector(TRIG_PIN, ECHO_PIN);

void setup() {
    Serial.begin(9600);
    detector.begin();
}

void loop() {
    float distance = detector.getDistance();
    Serial.print("Distance (cm): ");
    Serial.println(distance);
    delay(1000);
}
