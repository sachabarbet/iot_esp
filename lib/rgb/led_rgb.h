#ifndef LED_RGB_H
#define LED_RGB_H

#include <Arduino.h>
#include "timer_manager.h"

// Définition des broches de la LED RGB
#define RED_PIN 32
#define GREEN_PIN 33
#define BLUE_PIN 25

// États possibles de la LED
enum LedState {
    LED_OFF,
    LED_ON
};

// Structure pour gérer une LED avec un Timer basé sur un compteur
struct Led {
    LedState state;
    Timer timer;  // Utilisation du Timer avec compteur

    Led(unsigned long threshold = 500);  // Constructeur
};

// Déclarations des fonctions
void led_init();
void setRgbLedColor(int red, int green, int blue);
void updateLed(Led &led);
void setLedState(Led &led, LedState newState, unsigned long threshold);

#endif
