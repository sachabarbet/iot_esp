#include "led_rgb.h"

Led::Led(unsigned long threshold) : timer(threshold) {
    state = LED_OFF;
}

void led_init() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    setRgbLedColor(0, 0, 0);  

void setRgbLedColor(int red, int green, int blue) {
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
}

void setLedState(Led &led, LedState newState) {
    led.state = newState;
}
