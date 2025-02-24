#include "timer_manager.h"

// Constructeur : Initialise le compteur et le seuil
Timer::Timer(unsigned long thresholdValue) {
    threshold = thresholdValue;
    counter = 0;
}

// Vérifie si le timer a atteint son seuil
bool Timer::isExpired() {
    if (counter >= threshold) {
        counter = 0; // Réinitialisation après expiration
        return true;
    }
    return false;
}

// Réinitialise le compteur
void Timer::reset() {
    counter = 0;
}

// Incrémente le compteur
void Timer::increment() {
    counter++;
}

// Récupère la valeur du compteur
unsigned long Timer::getCounter() const {
    return counter;
}

// Récupère la valeur du seuil de mise à jour
unsigned long Timer::getThreshold() const {
    return threshold;
}
