#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <Arduino.h>

// Classe Timer utilisant un compteur au lieu de `millis()`
class Timer {
private:
    unsigned long counter;    // Compteur interne
    unsigned long threshold;  // Seuil pour l'activation
    bool expired;             // Indique si le timer a atteint son seuil

public:
    Timer(unsigned long thresholdValue = 1000);  // Constructeur avec seuil par défaut

    bool isExpired();  // Vérifie si le seuil est atteint
    void reset();  // Réinitialise le compteur
    void increment();  // Incrémente le compteur
};

#endif
