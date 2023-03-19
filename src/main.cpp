#include <Arduino.h>
#include <Definitions.hpp>
#include <CommandProtocol.hpp>
#include <RelayDriver.hpp>
#include <PWMOutputDriver.hpp>
#include <MotorDriver.hpp>

//PCA9685 Drivers
Adafruit_PWMServoDriver g_pca[_PCA_NUM] = {Adafruit_PWMServoDriver(_ADDR_PCA9685_0), 
                                           Adafruit_PWMServoDriver(_ADDR_PCA9685_1)};

//Relay Drivers
RelayDriver g_rly[_RELAY_NUM] = {RelayDriver(&g_pca[0], _PIN_RELAY_0), 
                                 RelayDriver(&g_pca[0], _PIN_RELAY_1),
                                 RelayDriver(&g_pca[0], _PIN_RELAY_2),
                                 RelayDriver(&g_pca[0], _PIN_RELAY_3),
                                 RelayDriver(&g_pca[0], _PIN_RELAY_4)};

//PWM Drivers
PWMOutputDriver g_pwm[_PWM_NUM] = {PWMOutputDriver(&g_pca[1], _PIN_PWM_0, 12), 
                                   PWMOutputDriver(&g_pca[1], _PIN_PWM_1, 12),
                                   PWMOutputDriver(&g_pca[1], _PIN_PWM_2, 5),
                                   PWMOutputDriver(&g_pca[1], _PIN_PWM_3, 5),
                                   PWMOutputDriver(&g_pca[1], _PIN_PWM_4, 5)};

//Motor Drivers
MotorDriver g_mtr[_MTR_NUM] = {MotorDriver(&g_pca[0], _PIN_ENABLE_MTR1, _PIN_IN1_MTR1, _PIN_IN2_MTR1),
                               MotorDriver(&g_pca[0], _PIN_ENABLE_MTR2, _PIN_IN1_MTR2, _PIN_IN2_MTR2),
                               MotorDriver(&g_pca[0], _PIN_ENABLE_MTR3, _PIN_IN1_MTR3, _PIN_IN2_MTR3),
                               MotorDriver(&g_pca[1], _PIN_ENABLE_MTR4, _PIN_IN1_MTR4, _PIN_IN2_MTR4),
                               MotorDriver(&g_pca[1], _PIN_ENABLE_MTR5, _PIN_IN1_MTR5, _PIN_IN2_MTR5),
                               MotorDriver(&g_pca[1], _PIN_ENABLE_MTR6, _PIN_IN1_MTR6, _PIN_IN2_MTR6)};

//Command interface
CommandProtocol g_cmd_interface;
CommandProtocol::Command g_cmd;

// Wakeup Pin
bool g_wkup_state;

void
setWakeup(bool state)
{
  g_wkup_state = state;
  digitalWrite(_PIN_PWR, state);
}

bool
getInfo(CommandProtocol::Command& cmd)
{
  if (cmd.cmd_type != CMD_TYPE_GET)
    return false;
  
  switch (cmd.dev)
  {
    case CMD_DEV_MOTOR:
      if (cmd.dev_num > _MTR_NUM)
        return false;

      cmd.cmd_type = CMD_TYPE_INFO;
      cmd.val[0] = (uint16_t)g_mtr[cmd.dev_num].getDirection();
      cmd.val[1] = (uint16_t)g_mtr[cmd.dev_num].getSpeed();
      g_cmd_interface.sendCommand(cmd);
      return true;
    
    case CMD_DEV_PWM:
      if (cmd.dev_num > _PWM_NUM)
        return false;

      cmd.cmd_type = CMD_TYPE_INFO;
      cmd.val[0] = (uint16_t)g_pwm[cmd.dev_num].state();
      g_cmd_interface.sendCommand(cmd);
      return true;
    
    case CMD_DEV_RELAY:
      if (cmd.dev_num > _RELAY_NUM)
        return false;

      cmd.cmd_type = CMD_TYPE_INFO;
      cmd.val[0] = (uint16_t)g_rly[cmd.dev_num].state();
      g_cmd_interface.sendCommand(cmd);
      return true;
    
    case CMD_DEV_WAKEUP:
      cmd.cmd_type = CMD_TYPE_INFO;
      cmd.val[0] = (uint16_t)g_wkup_state;
      g_cmd_interface.sendCommand(cmd);
      return true;

    default:
      g_cmd_interface.sendError("Unsuported device.");
      return false;
  }
}

bool
commandDevice(CommandProtocol::Command& cmd)
{
  if (cmd.cmd_type != CMD_TYPE_SET)
    return false;

  switch (cmd.dev)
  {
    case CMD_DEV_MOTOR:
      if (cmd.dev_num > _MTR_NUM)
        return false;
        
      g_mtr[cmd.dev_num].setDirection((bool)cmd.val[0]);
      g_mtr[cmd.dev_num].setSpeed(cmd.val[1]);
      return true;
    
    case CMD_DEV_PWM:
      if (cmd.dev_num > _PWM_NUM)
        return false;

      g_pwm[cmd.dev_num].setOutput(cmd.val[0]);
      return true;
    
    case CMD_DEV_RELAY:
      if (cmd.dev_num > _RELAY_NUM)
        return false;

      g_rly[cmd.dev_num].switchTo(cmd.val[0]);
      return true;
    
    case CMD_DEV_WAKEUP:
      g_cmd_interface.sendOk();

      setWakeup((bool)cmd.val[0]);
      return true;

    default:
      g_cmd_interface.sendError("Unsuported device.");
      return false;
  }

  return false;
}

void
setupDevices()
{
  // Set power pin
  pinMode(_PIN_PWR, OUTPUT);
  setWakeup(LOW);

  // Begin PCA9685 Drivers
  for (uint8_t i = 0; i < _PCA_NUM; i++)
  {
    g_pca[i].begin();
    delay(100);
  }

  // Set Relays and PWMs to OFF
  for (uint8_t i = 0; i < _RELAY_NUM; i++)
  {
    g_rly[i].turnOff();
    delay(100);
    g_pwm[i].turnOff();
    delay(100);
  }

  // Activate motors
  for (uint8_t i = 0; i < 1; i++)
  {
    g_mtr[i].begin();
    delay(100);
  }
}

void setup() 
{
  // Serial interface
  g_cmd_interface.begin(9600);

  // Devices
  setupDevices();

  setWakeup(true);
  g_mtr[0].setDirection(true);
  g_mtr[0].setSpeed(4000);
  g_mtr[0].turnOn();
}

void loop() 
{
  // bool res = g_cmd_interface.receiveCommand(g_cmd);

  // if (res)
  // {
  //   Serial.print("Cmd_type: ");Serial.println(g_cmd.cmd_type);
  //   Serial.print("Dev: ");Serial.println(g_cmd.dev);
  //   Serial.print("Dev_num: ");Serial.println(g_cmd.dev_num);
  //   Serial.print("Val[0]: ");Serial.println(g_cmd.val[0]);
  //   Serial.print("Val[1]: ");Serial.println(g_cmd.val[1]);
  //   Serial.print("\n\n");
  // }

  if (g_cmd_interface.receiveCommand(g_cmd))
  {
    if (commandDevice(g_cmd))
      g_cmd_interface.sendOk();
    else if (getInfo(g_cmd))
      g_cmd_interface.sendOk();
    else
      g_cmd_interface.sendError("Unsuported message type.");
  }
}