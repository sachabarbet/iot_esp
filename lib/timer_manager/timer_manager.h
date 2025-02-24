#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <Arduino.h>

// Classe Timer utilisant un compteur
class Timer {
private:
    unsigned long counter;    // Compteur interne
    unsigned long threshold;  // Seuil pour l'activation

public:
    Timer(unsigned long thresholdValue = 500);  // Constructeur avec seuil par défaut

    bool isExpired();  // Vérifie si le seuil est atteint
    void reset();  // Réinitialise le compteur
    void increment();  // Incrémente le compteur


    // Getters pour récupérer les valeurs du Timer
    unsigned long getCounter() const;   // Récupère la valeur du compteur
    unsigned long getThreshold() const; // Récupère le seuil de mise à jour
};

#endif
