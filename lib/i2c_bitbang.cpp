#include "i2c_bitbang.h"

// Petite pause pour la synchronisation I2C
#define I2C_DELAY() delayMicroseconds(4)

// Fonction pour configurer l'I2C en mode inactif
void i2c_init() {
    pinMode(SDA_PIN, OUTPUT);
    pinMode(SCL_PIN, OUTPUT);
    digitalWrite(SDA_PIN, HIGH);
    digitalWrite(SCL_PIN, HIGH);
}

// Générer une condition de Start I2C
void i2c_start() {
    digitalWrite(SDA_PIN, LOW);
    I2C_DELAY();
    digitalWrite(SCL_PIN, LOW);
}

// Générer une condition de Stop I2C
void i2c_stop() {
    digitalWrite(SDA_PIN, LOW);
    digitalWrite(SCL_PIN, HIGH);
    I2C_DELAY();
    digitalWrite(SDA_PIN, HIGH);
}

// Écrire un octet sur le bus I2C
bool i2c_write(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(SDA_PIN, (data & 0x80) ? HIGH : LOW);
        data <<= 1;
        I2C_DELAY();
        digitalWrite(SCL_PIN, HIGH);
        I2C_DELAY();
        digitalWrite(SCL_PIN, LOW);
    }

    // Lecture de l'ACK du capteur
    pinMode(SDA_PIN, INPUT);
    digitalWrite(SCL_PIN, HIGH);
    I2C_DELAY();
    bool ack = !digitalRead(SDA_PIN);
    digitalWrite(SCL_PIN, LOW);
    pinMode(SDA_PIN, OUTPUT);
    return ack;
}

// Lire un octet du bus I2C avec ACK/NACK
uint8_t i2c_read(bool ack) {
    uint8_t data = 0;
    pinMode(SDA_PIN, INPUT);

    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(SCL_PIN, HIGH);
        I2C_DELAY();
        data = (data << 1) | digitalRead(SDA_PIN);
        digitalWrite(SCL_PIN, LOW);
    }

    // Envoyer ACK/NACK
    pinMode(SDA_PIN, OUTPUT);
    digitalWrite(SDA_PIN, ack ? LOW : HIGH);
    digitalWrite(SCL_PIN, HIGH);
    I2C_DELAY();
    digitalWrite(SCL_PIN, LOW);
    
    return data;
}

// Lire la luminosité depuis le BH1750
uint16_t readBH1750() {
    i2c_start();
    if (!i2c_write(BH1750_ADDR << 1) || !i2c_write(BH1750_CMD_HRES)) {
        i2c_stop();
        Serial.println("Erreur : Écriture échouée !");
        return 0;
    }
    i2c_stop();

    delay(180);  // Temps de mesure du capteur

    // Lecture des 2 octets
    i2c_start();
    if (!i2c_write((BH1750_ADDR << 1) | 1)) {
        i2c_stop();
        Serial.println("Erreur : Lecture échouée !");
        return 0;
    }
    uint8_t msb = i2c_read(true);
    uint8_t lsb = i2c_read(false);
    i2c_stop();

    return ((msb << 8) | lsb) / 1.2;  // Conversion en lux
}
