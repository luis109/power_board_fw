#include "Arduino.h"
#include "PWMOutputDriver.hpp"
#include "Adafruit_PWMServoDriver.h"

PWMOutputDriver::PWMOutputDriver(Adafruit_PWMServoDriver* driver, uint8_t pin, uint8_t voltage):
	m_driver(driver), 
	m_pin(pin),
	m_voltage(voltage)
{
}

void
PWMOutputDriver::setOutput(uint16_t state)
{
	// Checks
	if (state > 4095)
	{
		m_driver->setPWM(m_pin, 4096, 0);
		return;
	}
	if (state < 0)
	{
		m_driver->setPWM(m_pin, 0, 4096);
		return;
	}

	// Set state
	m_driver->setPWM(m_pin, 0, state);
}

void
PWMOutputDriver::turnOn()
{
	// Set fully on
	m_driver->setPWM(m_pin, 4096, 0);
}

void
PWMOutputDriver::turnOff()
{
	// Set fully on
	m_driver->setPWM(m_pin, 0, 4096);
}

// Test the values this returns
uint8_t
PWMOutputDriver::state()
{
	return m_driver->getPWM(m_pin);
}