#include <Arduino.h>
#include "led_rgb.h"
#include "i2c_bitbang.h"

#define LIGHT_THRESHOLD 200  // Seuil de luminosité pour activer la LED

Led myLed(50); // LED mise à jour toutes les 500 cycles

// Fonction pour mettre à jour l'état de la LED en fonction de la luminosité
void updateLed(Led &led) {
    led.timer.increment();  // Incrémente le compteur

    if (led.timer.isExpired()) {  // Vérifie si le seuil du Timer est atteint
        uint16_t lux = readBH1750();  // Lecture de la luminosité
        Serial.print("Luminosité : ");
        Serial.print(lux);
        Serial.println(" lux");

        // ✅ LED s'allume quand la luminosité est FAIBLE
        if (lux < LIGHT_THRESHOLD) {  
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

// Afficher l'état de l'objet LED
void printLedStatus(const Led &led) {
    Serial.print("LED State: ");
    Serial.print((led.state == LED_ON) ? "ON" : "OFF");
    Serial.print(" | Timer Counter: ");
    Serial.print(led.timer.getCounter());  // Récupère la valeur du compteur
    Serial.print(" / ");
    Serial.print(led.timer.getThreshold()); // Affiche le seuil de mise à jour
    Serial.println(" cycles");
}

void setup() {
    Serial.begin(115200);
    i2c_init();
    led_init();
    
    Serial.println("Gestion automatique de la LED en fonction de la luminosité");
}

void loop() {
    updateLed(myLed);  // Met à jour l'état de la LED en fonction de la luminosité
    printLedStatus(myLed);  // Affiche les informations de l'objet `Led`
    
    delay(100);  // Petite pause pour éviter un affichage trop rapide
}
