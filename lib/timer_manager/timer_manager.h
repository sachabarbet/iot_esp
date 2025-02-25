#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <Arduino.h>

class Timer {
private:
    unsigned long counter;    
    unsigned long threshold;  

public:
    Timer(unsigned long thresholdValue = 500); 

    bool isExpired();  
    void reset();  
    void increment();


    unsigned long getCounter() const;
    unsigned long getThreshold() const;
};

#endif
