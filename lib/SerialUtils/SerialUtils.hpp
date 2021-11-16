#ifndef SerialUtils_hpp
#define SerialUtils_hpp

#include <Arduino.h>

enum
{
  FLD_DEV = 0,      // Device: MOTOR, RELAY, PWM
  FLD_DEV_NUM = 1,  // Devide number
  FLD_CMD = 2,      // Command: SET, GET
  FLD_PARAM = 3,    // Parameter
  FLD_VAL = 4,    // Value
  FLD_NUM
};

class SerialUtils
{
  public:
    static void 
    begin(int baud, int timeout = 1000)
    {
      Serial.begin(baud);
      if (timeout != 1000)
        Serial.setTimeout(timeout);

      while(!Serial){}
    }

    // Return number of characters read to buffer
    static uint8_t 
    receiveMessage(char* bfr, uint8_t bfr_length)
    {
      if (Serial.available() > 0) 
      {
        uint8_t size = Serial.readBytesUntil('\n', bfr, bfr_length);

        if (size > bfr_length - 1)
          return 0;

        bfr[size] = '\0';
        return size;
      }
      
      return 0;
    }

    static size_t
    sendMessage(const char* msg)
    {
      return Serial.print(msg);
    }
};

#endif