#include <Arduino.h>
#include "led_rgb.h"
#include "i2c_bitbang.h"
#include "detector.h"

#define LIGHT_THRESHOLD 200  // Seuil de luminosité pour activer la LED
#define TRIG_PIN 5           // Broche Trigger du capteur de mouvement
#define ECHO_PIN 18          // Broche Echo du capteur de mouvement
#define CYCLE 500            // Durée d'un cycle en ms

Led myLed(CYCLE);  // LED mise à jour toutes les CYCLE cycles
Detector myDetector(TRIG_PIN, ECHO_PIN, CYCLE);  // Détecteur mis à jour toutes les CYCLE cycles

// Fonction pour mettre à jour l'état de la LED en fonction de la luminosité et du mouvement
void updateLed(Led &led, Detector &detector) {
    led.timer.increment();  // Incrémente le compteur du Timer

    if (led.timer.isExpired()) {  // Vérifie si le seuil du Timer est atteint
        uint16_t lux = readBH1750();  // Lecture de la luminosité
        detector.update();  // Mise à jour du capteur de mouvement
        int distance = detector.getDistance();  // Récupération de la distance

        Serial.print("Luminosité : ");
        Serial.print(lux);
        Serial.print(" lux | Distance : ");
        Serial.print(distance);
        Serial.println(" cm");

        // ✅ La LED s'allume si la luminosité est FAIBLE ET si un mouvement est détecté
        if (lux < LIGHT_THRESHOLD && (distance > 0 && distance < 150)){  // Détection de mouvement < 400 cm
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

// Afficher l'état de l'objet LED et Detector
void printStatus(const Led &led, const Detector &detector) {
    Serial.print("LED State: ");
    Serial.print((led.state == LED_ON) ? "ON" : "OFF");
    Serial.print(" | Timer Counter: ");
    Serial.print(led.timer.getCounter());  // Récupère la valeur du compteur
    Serial.print(" / ");
    Serial.print(led.timer.getThreshold()); // Affiche le seuil de mise à jour
    Serial.print(" cycles | Detector State: ");
    Serial.println((detector.getState() == Detector::SENDING) ? "SENDING" : (detector.getState() == Detector::MEASURING) ? "MEASURING" : "IDLE");
}

void setup() {
    Serial.begin(115200);
    i2c_init();
    led_init();
    myDetector.begin();
    
    Serial.println("Gestion automatique de la LED en fonction de la luminosité et du mouvement");
}

void loop() {
    updateLed(myLed, myDetector);  // Met à jour l'état de la LED en fonction de la luminosité et du mouvement
   // printStatus(myLed, myDetector);  
   // Affiche les informations de l'objet `Led` et `Detector`
    
    delay(100);  // Petite pause pour éviter un affichage trop rapide
}
