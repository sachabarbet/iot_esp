#include "detector.h"

Detector::Detector(int trig, int echo, unsigned long interval)
    : trigPin(trig), echoPin(echo), state(IDLE), stateTimer(interval), lastDistance(0) {}

void Detector::begin() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void Detector::update() {
    stateTimer.increment();

    switch (state) {
        case IDLE:
            if (stateTimer.isExpired()) {
                state = MEASURING;
                stateTimer.reset();
            }
            break;

        case MEASURING:
            lastDistance = getDistance();
            stateTimer.reset();
            state = SENDING;
            break;

        case SENDING:
            Serial.print("Distance mesurée : ");
            Serial.print(lastDistance);
            Serial.println(" cm");

            state = IDLE;
            stateTimer.reset();
            break;
    }
}

int Detector::getDistance() {
    long duration;
    int distance_cm;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(TRIG_PULSE_DURATION_US);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH, 30000); 

    if (duration == 0) {
        return -1;
    }

    distance_cm = duration * SOUND_SPEED / 2.0;  

    return distance_cm;
}

Detector::DetectorState Detector::getState() const {
    return state;
}
