#ifndef MotorDriver_hpp
#define MotorDriver_hpp

#include "Arduino.h"
#include "Adafruit_PWMServoDriver.h"

class MotorDriver
{
  public:
    MotorDriver(Adafruit_PWMServoDriver* driver, uint8_t enable_pin, uint8_t phase_pin, uint8_t sleep_pin);

    void 
    begin();

    void 
    brake();

    void 
    turnOn();

    void 
    turnOff();

    void 
    setSpeed(uint16_t speed);

    void 
    setDirection(bool direction);

    void 
    switchDirection();

    uint8_t 
    getSpeed();

    bool 
    getDirection();

  private:
    Adafruit_PWMServoDriver* m_driver;
    uint8_t m_enable_pin;
    uint8_t m_in1_pin;
    uint8_t m_in2_pin;
};

#endif