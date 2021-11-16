#ifndef RelayDriver_hpp
#define RelayDriver_hpp

#include "Arduino.h"
#include <Wire.h>
#include "Adafruit_PWMServoDriver.h"

class RelayDriver
{
  public:
    RelayDriver(Adafruit_PWMServoDriver* driver, uint8_t pin);
    void switchTo(bool state);
    void turnOn();
    void turnOff();
    void switchState();
    bool state();

  private:
    Adafruit_PWMServoDriver* m_driver;
    uint8_t m_pin;
};

#endif