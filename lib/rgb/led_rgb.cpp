#include "led_rgb.h"

// Création d'une instance de la LED avec un seuil de 1000 cycles
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

// Fonction pour changer l'état de la LED avec un Timer
void setLedState(Led &led, LedState newState, unsigned long threshold) {
    led.state = newState;
    led.timer.reset();
}

// Fonction pour mettre à jour l'état de la LED en fonction du compteur
void updateLed(Led &led) {
    led.timer.increment();  // Incrémente le compteur à chaque cycle

    if (led.timer.isExpired()) {  // Vérifie si le seuil est atteint
        switch (led.state) {
            case LED_OFF:
                setRgbLedColor(0, 0, 0);  // Éteindre la LED
                break;
            case LED_ON:
                setRgbLedColor(255, 0, 0);  // Allumer en rouge
                break;
        }
        led.timer.reset();  // Réinitialise le compteur pour le prochain cycle
    }
}
