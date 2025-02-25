#ifndef I2C_BITBANG_H
#define I2C_BITBANG_H

#include <Arduino.h>

#define SDA_PIN 21
#define SCL_PIN 22

#define BH1750_ADDR 0x23  
#define BH1750_CMD_HRES 0x10

void i2c_init();
void i2c_start();
void i2c_stop();
bool i2c_write(uint8_t data);
uint8_t i2c_read(bool ack);
uint16_t readBH1750();

#endif
