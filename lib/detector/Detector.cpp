#include "Detector.h"

Detector::Detector(int trig, int echo) : trigPin(trig), echoPin(echo) {}

void Detector::begin() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

float Detector::getDistance() {
    long duration;
    float distance_cm;

    // Génération de l'impulsion ultrasonique
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(TRIG_PULSE_DURATION_US);
    digitalWrite(trigPin, LOW);

    // Lecture de la durée du signal de l'écho
    duration = pulseIn(echoPin, HIGH);

    // Calcul de la distance en cm
    distance_cm = duration * SOUND_SPEED / 2 * 0.0001; 

    return distance_cm;
}
