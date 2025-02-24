#include "led_rgb.h"

// Création d'une instance de la LED avec un seuil de 500 cycles
Led::Led(unsigned long threshold) : timer(threshold) {
    state = LED_OFF;
}

// Initialisation des broches de la LED RGB
void led_init() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    setRgbLedColor(0, 0, 0);  // Éteindre la LED au démarrage
}

// Fonction pour définir la couleur de la LED RGB
void setRgbLedColor(int red, int green, int blue) {
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
}

// Fonction pour changer l'état de la LED
void setLedState(Led &led, LedState newState) {
    led.state = newState;
}
