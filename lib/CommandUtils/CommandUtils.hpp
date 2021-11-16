#ifndef CommandUtils_hpp
#define CommandUtils_hpp

#include <SerialUtils.hpp>

#define CMDUTIL_BUFFER_SIZE 128

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

enum
{
  RPLY_OK = 'O',
  RPLY_FAILED = 'F'
};

enum
{
  DELIM_INITIALIZER = '$',
  DELIM_TERMINATOR = '*'
};

struct
Command
{
	char dev;
  uint8_t dev_num;
  char cmd_type;
  uint8_t val[2];

  Command()
  {
    reset();
  }

  void
  reset()
  {
    dev = '\0';
    dev_num = -1;
    cmd_type = '\0';
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

class CommandUtils
{
  public:
    void begin(int baud);

    bool receiveCommand(Command& cmd);
    bool sendCommand(Command& cmd);
    bool sendReply(uint8_t rply);
    bool sendHeartbeat();

  private:
    bool decode(Command& cmd);
    bool encode(Command& cmd);

    char m_bfr[CMDUTIL_BUFFER_SIZE];
    char m_msg[CMDUTIL_BUFFER_SIZE];
};

#endif