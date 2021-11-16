#include "Arduino.h"
#include "MotorDriver.hpp"
#include "Adafruit_PWMServoDriver.h"

MotorDriver::MotorDriver(Adafruit_PWMServoDriver* driver, uint8_t enable_pin, uint8_t phase_pin, uint8_t sleep_pin):
	m_driver(driver), 
	m_enable_pin(enable_pin),
	m_phase_pin(phase_pin),
	m_sleep_pin(sleep_pin)
{
}

void
MotorDriver::begin()
{
	sleep();
	turnOff();
	setDirection(0);
}

void
MotorDriver::wakeup()
{
	m_driver->setPWM(m_sleep_pin, 4096, 0);
}

void
MotorDriver::sleep()
{
	m_driver->setPWM(m_sleep_pin, 0, 4096);
}

void
MotorDriver::turnOn()
{
	// Set fully on
	m_driver->setPWM(m_enable_pin, 4096, 0);
}

void
MotorDriver::turnOff()
{
	// Set fully on
	m_driver->setPWM(m_enable_pin, 0, 4096);
}

void
MotorDriver::setDirection(bool direction)
{
	if (direction)
		m_driver->setPWM(m_phase_pin, 4096, 0);
	else
		m_driver->setPWM(m_phase_pin, 0, 4096);
}

void
MotorDriver::switchDirection()
{
	setDirection(!getDirection());
}

void
MotorDriver::setSpeed(uint16_t speed)
{
	// Checks
	if (speed >= 4095)
	{
		m_driver->setPWM(m_enable_pin, 4096, 0);
		return;
	}
	else if (speed < 0)
	{
		m_driver->setPWM(m_enable_pin, 0, 4096);
		return;
	}

	// Set state
	m_driver->setPWM(m_enable_pin, 0, speed);
}

// Test the values this returns
uint8_t
MotorDriver::getSpeed()
{
	return m_driver->getPWM(m_enable_pin);
}

// Test the values this returns
bool
MotorDriver::getDirection()
{
	return m_driver->getPWM(m_phase_pin) != 0;
}