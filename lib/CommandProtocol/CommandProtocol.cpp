#include <CommandProtocol.hpp>

uint8_t xor_check(char *data)
{
  uint8_t csum = 0x00;
  uint8_t t = 0;
  while (data[t] != '\0')
  {
    csum ^= data[t];
    t++;
  }
  return csum;
}

void
CommandProtocol::begin(int baud)
{
  SerialUtils::begin(baud);
}


bool 
CommandProtocol::receiveString(char* str)
{
  uint8_t length = SerialUtils::receiveMessage(str, CP_BUFFER_SIZE);

  if (length == 0)
    return false;
  
  if (str[0] != '$')
    return false;

  uint16_t csum;
  sscanf(&str[1], "%[^*]*%x%*s", str, &csum);

  // Serial.print("Message: ");
  // Serial.println(str);
  // Serial.print("Checksum (msg): ");
  // Serial.println((int)csum);
  // Serial.print("Checksum (calc): ");
  // Serial.println((int)xor_check(&m_bfr[1]));

  if (csum != (uint16_t)xor_check(str))
    return false;

  return true;
}

bool 
CommandProtocol::sendString(char* msg)
{
  char bfr[CP_BUFFER_SIZE];
  if (sprintf(bfr, "$%s*%02x\n", msg, xor_check(msg)) < 0)
    return false;

  SerialUtils::sendMessage(bfr);
  return true;
}

bool
CommandProtocol::decode(Command& cmd, char* str)
{
  //Clear command
  cmd.reset();

  uint8_t filled_args;
  filled_args = sscanf(str, "%c,%c,%u,%u,%u", &cmd.m_type,
                                              &cmd.m_dev,
                                              &cmd.m_dev_num,
                                              &cmd.m_val[0],
                                              &cmd.m_val[1]);
  
  // Sanity checks
  if (filled_args < 3)
    return false;
  
  if (cmd.m_type == CMD_TYPE_GET)
  {
    if (filled_args != 3)
      return false;
  }
  else
  {
    if (cmd.m_dev == CMD_DEV_MOTOR)
    {
      if (filled_args != 5)
        return false;
    }
    else if (filled_args != 4)
    {
      return false;
    }
  }

  return true;
}

bool
CommandProtocol::encode(Command& cmd, char* str)
{
  uint8_t lenght;
  lenght = sprintf(str, "%c,%c", cmd.m_type,
                                 cmd.m_dev);

  if (lenght < 0)                          
    return false;

  if (cmd.m_dev_num != 65535) // Invalid (uint16_t)(-1)
      lenght += sprintf(&str[lenght], ",%u", cmd.m_dev_num);

  for (uint8_t i = 0; i < 2; i++)
  {
    if (cmd.m_val[i] != 65535) // Invalid (uint16_t)(-1)
      lenght += sprintf(&str[lenght], ",%u", cmd.m_val[i]);
  }

  return true;
}

bool 
CommandProtocol::receiveCommand(Command& cmd)
{
  char bfr[CP_BUFFER_SIZE];
  if (!receiveString(bfr))
    return false;

  if (!decode(cmd, bfr))
    return false;

  return true;
}

bool 
CommandProtocol::sendCommand(Command& cmd)
{
  //Should only send CMD_TYPE_INFO
  if (cmd.m_type != CMD_TYPE_INFO)
    return false;
  
  char str[CP_BUFFER_SIZE];
  if (!encode(cmd, str))
    return false;

  return sendString(str);
}

bool
CommandProtocol::sendOk()
{
  char msg[] = "OK";
  return sendString(msg);
}

bool
CommandProtocol::sendError(const char* error_msg)
{
  char msg[CP_BUFFER_SIZE];
  sprintf(msg, "ERROR,%s", error_msg);
  return sendString(msg);
}

bool
CommandProtocol::sendHeartbeat()
{
  char msg[] = "HB";
  return sendString(msg);
}