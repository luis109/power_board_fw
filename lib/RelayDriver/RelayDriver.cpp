#include "Arduino.h"
#include "RelayDriver.hpp"
#include <Wire.h>
#include "Adafruit_PWMServoDriver.h"

RelayDriver::RelayDriver(Adafruit_PWMServoDriver* driver, uint8_t pin):
	m_driver(driver), 
	m_pin(pin)
{
}

void
RelayDriver::switchTo(bool state)
{
	if (state)
		m_driver->setPWM(m_pin, 4096, 0); // 4096 (0001 0000 0000 0000) Means fully on
	else
		m_driver->setPWM(m_pin, 0, 4096); // 4096 (0001 0000 0000 0000) Means fully off
}

void
RelayDriver::turnOn()
{
  switchTo(true);
}

void
RelayDriver::turnOff()
{
	switchTo(false);
}

void
RelayDriver::switchState()
{
	switchTo(!state());
}

// Test the values this returns
bool
RelayDriver::state()
{
	return m_driver->getPWM(m_pin) > 0;
}