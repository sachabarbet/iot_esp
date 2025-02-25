#include "timer_manager.h"

Timer::Timer(unsigned long thresholdValue) {
    threshold = thresholdValue;
    counter = 0;
}

bool Timer::isExpired() {
    if (counter >= threshold) {
        counter = 0; 
        return true;
    }
    return false;
}

void Timer::reset() {
    counter = 0;
}

void Timer::increment() {
    counter++;
}

unsigned long Timer::getCounter() const {
    return counter;
}

unsigned long Timer::getThreshold() const {
    return threshold;
}
