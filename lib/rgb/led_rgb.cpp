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

// Fonction pour mettre à jour l'état de la LED en fonction de la luminosité
void updateLed(Led &led) {
    led.timer.increment();  // Incrémente le compteur

    if (led.timer.isExpired()) {  // Vérifie si le seuil du Timer est atteint
        uint16_t lux = readBH1750();  // Lecture de la luminosité
        Serial.print("Luminosité : ");
        Serial.print(lux);
        Serial.println(" lux");

        if (lux >= LIGHT_THRESHOLD) {
            setLedState(led, LED_ON);
        } else {
            setLedState(led, LED_OFF);
        }

        // Appliquer l’état de la LED
        if (led.state == LED_ON) {
            setRgbLedColor(255, 0, 0);  // Rouge si allumé
        } else {
            setRgbLedColor(0, 0, 0);  // Éteint
        }
    }
}
