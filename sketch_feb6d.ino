#include <Wire.h>
#include <BH1750.h>

#define SDA_PIN 1  // Modifier selon ton ESP32-S3
#define SCL_PIN 2  // Modifier selon ton ESP32-S3

BH1750 lightMeter;

void setup() {
    Serial.begin(115200);
    
    // Initialisation du bus I2C
    Wire.begin(SDA_PIN, SCL_PIN);

    // Initialisation du capteur BH1750
    if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
        Serial.println("Capteur BH1750 prêt !");
    } else {
        Serial.println("Échec de l'initialisation du BH1750 !");
    }
}

void loop() {
    float lux = lightMeter.readLightLevel();
    Serial.print("Luminosité : ");
    Serial.print(lux);
    Serial.println(" lx");

    delay(500); // Lecture toutes les 500ms
}
