#ifndef PWMOutputDriver_hpp
#define PWMOutputDriver_hpp

#include "Arduino.h"
#include "Adafruit_PWMServoDriver.h"

class PWMOutputDriver
{
  public:
    PWMOutputDriver(Adafruit_PWMServoDriver* driver, uint8_t pin, uint8_t voltage = 0);
    void setOutput(uint16_t val);
    void turnOn();
    void turnOff();
    uint8_t state();

  private:
    Adafruit_PWMServoDriver* m_driver;
    uint8_t m_pin;
    uint8_t m_voltage;
};

#endif