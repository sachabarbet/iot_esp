#include <Arduino.h>
#include "i2c_bitbang.h"

void setup() {
    Serial.begin(115200);
    i2c_init();
    Serial.println("Test du capteur BH1750 avec I2C manuel");
}

void loop() {
    uint16_t lux = readBH1750();
    Serial.print("Luminosité : ");
    Serial.print(lux);
    Serial.println(" lux");

    delay(1000);
}
