#include <Arduino.h>
#include "led_rgb.h"
#include "i2c_bitbang.h"

Led myLed(500); // LED mise à jour toutes les 500 cycles

void setup() {
    Serial.begin(115200);
    i2c_init();
    led_init();
    
    Serial.println("Gestion automatique de la LED en fonction de la luminosité");
}

void loop() {
    updateLed(myLed);  // Met à jour l'état de la LED en fonction de la luminosité
}
