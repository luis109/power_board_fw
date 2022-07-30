#ifndef CommandProtocol_hpp
#define CommandProtocol_hpp

#include <SerialUtils.hpp>

#define CP_BUFFER_SIZE 128

enum
{
  DEV_MOTOR = 'M',
  DEV_PWM = 'P',
  DEV_RELAY = 'R'
};

enum
{
  CMD_TYPE_SET = 'S',
  CMD_TYPE_GET = 'G',
  CMD_TYPE_INFO = 'I'
};

struct
Command
{
  char cmd_type;
	char dev;
  uint8_t dev_num;
  uint8_t val[2];

  Command()
  {
    reset();
  }

  void
  reset()
  {
    cmd_type = '\0';
    dev = '\0';
    dev_num = -1;
    val[0] = -1;
    val[1] = -1;
  }

  void
  typeConvert(uint16_t aux[3])
  {
    dev_num = (uint8_t)aux[0];
    val[0] = (uint8_t)aux[1];
    val[1] = (uint8_t)aux[2];
  }
};

class CommandProtocol
{
  public:
    void begin(int baud);

    bool receiveCommand(Command& cmd);
    bool sendCommand(Command& cmd);
    bool sendError(char* error_msg);
    bool sendOk();
    bool sendHeartbeat();

  private:
    bool sendString(char* msg);
    bool decode(Command& cmd);
    bool encode(Command& cmd);

    char m_bfr[CP_BUFFER_SIZE];
    char m_msg[CP_BUFFER_SIZE];
};

#endif