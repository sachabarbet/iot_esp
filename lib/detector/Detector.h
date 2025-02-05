#ifndef DETECTOR_H
#define DETECTOR_H

#include <Arduino.h>

class Detector {
private:
    int trigPin;
    int echoPin;
    static constexpr float SOUND_SPEED = 340.0;  // Vitesse du son en m/s
    static constexpr int TRIG_PULSE_DURATION_US = 10;

public:
    Detector(int trig, int echo);
    void begin();
    float getDistance();
};

#endif
