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
Command g_cmd;

bool
stateMachine(Command& cmd)
{
  switch (cmd.dev)
  {
    case CMD_DEV_MOTOR:
      /* code */
      break;
    
    case CMD_DEV_PWM:
      /* code */
      break;
    
    case CMD_DEV_RELAY:
      if (cmd.dev_num > _RELAY_NUM)
        return false;

      if (cmd.cmd_type == CMD_TYPE_SET)
      {
        g_rly[cmd.dev_num].switchTo(cmd.val[0] != 0);
      }
      else if (cmd.cmd_type == CMD_TYPE_GET)
      {
        cmd.cmd_type = CMD_TYPE_INFO;
        cmd.val[0] = (uint8_t)g_rly[cmd.dev_num].state();

        g_cmd_interface.sendCommand(cmd);
      }
      break;
    
    default:
      return false;
  }

  return true;
}

void setup() 
{
  // Serial tests
  g_cmd_interface.begin(9600);
  // Serial.begin(9600);

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

  // // Activate motors
  // for (uint8_t i = 0; i < 1; i++)
  // {
  //   g_mtr[i].wakeup();
  //   delay(100);
  // }

  // g_rly[0].switchTo(true);
}

void loop() 
{
  bool res = g_cmd_interface.receiveCommand(g_cmd);

  if (res)
  {
    Serial.print("Dev: ");Serial.println(g_cmd.dev);
    Serial.print("Dev_num: ");Serial.println(g_cmd.dev_num);
    Serial.print("Cmd_type: ");Serial.println(g_cmd.cmd_type);
    Serial.print("Val[0]: ");Serial.println(g_cmd.val[0]);
    Serial.print("Val[1]: ");Serial.println(g_cmd.val[1]);
    Serial.print("\n\n");
  }

  stateMachine(g_cmd);
}