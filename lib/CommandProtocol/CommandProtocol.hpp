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
  uint16_t dev_num;
  uint16_t val[2];

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
    bool receiveString(char* msg);
    bool decode(Command& cmd, char* str);
    bool encode(Command& cmd, char* str);
};

#endif