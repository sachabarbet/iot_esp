#ifndef LED_RGB_H
#define LED_RGB_H

#include <Arduino.h>
#include "timer_manager.h"
#include "i2c_bitbang.h" 

#define RED_PIN 32
#define GREEN_PIN 33
#define BLUE_PIN 25

#define LIGHT_THRESHOLD 200

enum LedState {
    LED_OFF,
    LED_ON
};

struct Led {
    LedState state;
    Timer timer;  

    Led(unsigned long threshold = 500);  
};

void led_init();
void setRgbLedColor(int red, int green, int blue);
void setLedState(Led &led, LedState newState);

#endif
