#ifndef CommandProtocol_hpp
#define CommandProtocol_hpp

#include <SerialUtils.hpp>

#define CP_BUFFER_SIZE 128

enum
{
  CMD_DEV_MOTOR = 'M',
  CMD_DEV_PWM = 'P',
  CMD_DEV_RELAY = 'R',
  CMD_DEV_WAKEUP = 'W'
};

enum
{
  CMD_TYPE_SET = 'S',
  CMD_TYPE_GET = 'G',
  CMD_TYPE_INFO = 'I'
};
struct Command
{
  char m_type;
  char m_dev;
  uint16_t m_dev_num;
  uint16_t m_val[2];

  Command()
  {
    reset();
  }

  void
  reset()
  {
    m_type = '\0';
    m_dev = '\0';
    m_dev_num = -1;
    m_val[0] = -1;
    m_val[1] = -1;
  }
};

class CommandProtocol
{
  public:
    
    void begin(int baud);

    bool receiveCommand(Command& cmd);
    bool sendCommand(Command& cmd);
    bool sendError(const char* error_msg);
    bool sendOk();
    bool sendHeartbeat();

  private:
    bool sendString(char* msg);
    bool receiveString(char* msg);
    bool decode(Command& cmd, char* str);
    bool encode(Command& cmd, char* str);
};

#endif