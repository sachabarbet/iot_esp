#ifndef DETECTOR_H
#define DETECTOR_H

#include <Arduino.h>
#include "timer_manager.h"  

class Detector {
public:
    enum DetectorState {
        IDLE,       // Veille (en attente)
        MEASURING,  // Lecture en cours
        SENDING     // Envoi des données
    };

private:
    int trigPin; 
    int echoPin; 
    DetectorState state; 
    Timer stateTimer; 
    int lastDistance; 

    static constexpr float SOUND_SPEED = 0.0343;  
    static constexpr int TRIG_PULSE_DURATION_US = 10; 

public:
    Detector(int trig, int echo, unsigned long interval = 500);
    void begin();
    void update();  
    int getDistance(); 
    DetectorState getState() const;
};

#endif
