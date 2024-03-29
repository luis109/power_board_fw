#include "Arduino.h"
#include "MotorDriver.hpp"
#include "Adafruit_PWMServoDriver.h"

MotorDriver::MotorDriver(Adafruit_PWMServoDriver *driver, uint8_t enable_pin, uint8_t in1_pin, uint8_t in2_pin) : 
  m_driver(driver),
  m_enable_pin(enable_pin),
  m_in1_pin(in1_pin),
  m_in2_pin(in2_pin)
{
}

void MotorDriver::begin()
{
  turnOff();
  setDirection(0);
}

void MotorDriver::turnOn()
{
  // Set fully on
  m_driver->setPWM(m_enable_pin, 4096, 0);
}

void MotorDriver::turnOff()
{
  // Set fully off
  m_driver->setPWM(m_enable_pin, 0, 4096);
}

void MotorDriver::brake()
{
  m_driver->setPWM(m_in1_pin, 4096, 0);
  m_driver->setPWM(m_in2_pin, 4096, 0);
}

void MotorDriver::setDirection(bool direction)
{
  if (direction)
  {
    m_driver->setPWM(m_in1_pin, 0, 4096);
    m_driver->setPWM(m_in2_pin, 4096, 0);
  }
  else
  {
    m_driver->setPWM(m_in1_pin, 4096, 0);
    m_driver->setPWM(m_in2_pin, 0, 4096);
  }
}

void MotorDriver::switchDirection()
{
  setDirection(!getDirection());
}

void MotorDriver::setSpeed(uint16_t speed)
{
  // Speed comes as percentage from 0 to 100
  if (speed >= 100)
  {
    m_driver->setPWM(m_enable_pin, 4096, 0);
    return;
  }

  // Perform conversion to motor scale (0 to 4095)
  m_driver->setPWM(m_enable_pin, 0, map(speed, 0, 100, 0, 4095));
}

// Test the values this returns
// NOT WORKING
uint8_t
MotorDriver::getSpeed()
{
  return m_driver->getPWM(m_enable_pin);
}

// Test the values this returns
// NOT WORKING
bool MotorDriver::getDirection()
{
  return m_driver->getPWM(m_in1_pin) != 0;
}