#ifndef DETECTOR_H
#define DETECTOR_H

#include <Arduino.h>
#include "timer_manager.h"  // Utilisation du Timer pour gérer l'attente

class Detector {
public:
    // Définition des états possibles
    enum DetectorState {
        IDLE,       // Veille (en attente)
        MEASURING,  // Lecture en cours
        SENDING     // Envoi des données
    };

private:
    int trigPin;  // Broche Trigger du capteur
    int echoPin;  // Broche Echo du capteur
    DetectorState state;  // État actuel du capteur

    Timer stateTimer;  // Timer pour la gestion des états
    int lastDistance;  // Dernière distance mesurée

    static constexpr float SOUND_SPEED = 0.0343;  // Vitesse du son en cm/µs
    static constexpr int TRIG_PULSE_DURATION_US = 10;  // Durée de l'impulsion Trigger en µs

public:
    Detector(int trig, int echo, unsigned long interval = 500);
    void begin();
    void update();  // Gère la machine d'état
    int getDistance();  // Effectue la lecture
    DetectorState getState() const;
};

#endif
