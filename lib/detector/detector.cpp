#include "detector.h"

Detector::Detector(int trig, int echo, unsigned long interval)
    : trigPin(trig), echoPin(echo), state(IDLE), stateTimer(interval), lastDistance(0) {}

void Detector::begin() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

// Fonction qui gère la machine d'état
void Detector::update() {
    stateTimer.increment();  // Incrémente le compteur du Timer

    switch (state) {
        case IDLE:
            // Attente jusqu'à la prochaine mesure
            if (stateTimer.isExpired()) {
                state = MEASURING;
                stateTimer.reset();  // Réinitialiser le Timer pour la prochaine étape
            }
            break;

        case MEASURING:
            // Effectuer la mesure et stocker le résultat
            lastDistance = getDistance();
            stateTimer.reset();
            state = SENDING;
            break;

        case SENDING:
            // Envoi des données (ex: affichage ou transmission)
            Serial.print("Distance mesurée : ");
            Serial.print(lastDistance);
            Serial.println(" cm");

            // Retour à l'état IDLE après l'envoi
            state = IDLE;
            stateTimer.reset();
            break;
    }
}

// Fonction pour mesurer la distance avec le capteur à ultrasons
int Detector::getDistance() {
    long duration;
    int distance_cm;

    // Envoi d'une impulsion ultrasonique
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(TRIG_PULSE_DURATION_US);
    digitalWrite(trigPin, LOW);

    // Lecture de la durée du signal de l'écho
    duration = pulseIn(echoPin, HIGH, 30000);  // Timeout 30ms pour éviter blocage

    // Si la mesure est hors de portée
    if (duration == 0) {
        return -1;  // Indique une erreur de mesure
    }

    // Calcul de la distance en cm
    distance_cm = duration * SOUND_SPEED / 2.0;  // Divisé par 2 car aller-retour

    return distance_cm;
}

// Fonction pour récupérer l'état actuel du capteur
Detector::DetectorState Detector::getState() const {  // ✅ Ajout de `const`
    return state;
}
