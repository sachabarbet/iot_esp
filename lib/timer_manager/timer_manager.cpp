#include "timer_manager.h"

// Constructeur : Initialise le compteur et le seuil
Timer::Timer(unsigned long thresholdValue) {
    threshold = thresholdValue;
    counter = 0;
    expired = false;
}

// Vérifie si le timer a atteint son seuil
bool Timer::isExpired() {
    if (counter >= threshold) {
        expired = true;
        return true;
    }
    return false;
}

// Réinitialise le compteur
void Timer::reset() {
    counter = 0;
    expired = false;
}

// Incrémente le compteur
void Timer::increment() {
    if (!expired) {
        counter++;
    }
}
