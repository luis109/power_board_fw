#include <CommandProtocol.hpp>

uint8_t xor_check(char *data)
{
  uint8_t csum = 0x00;
  uint8_t t = 0;
  while (data[t] != '*')
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
  uint8_t length = SerialUtils::receiveMessage(m_bfr, CP_BUFFER_SIZE);

  if (length == 0)
    return false;
  
  if (m_bfr[0] != '$')
    return false;

  uint16_t csum;
  sscanf(&m_bfr[1], "%[^*]*%x%*s", str, &csum);

  // Serial.print("Message: ");
  // Serial.println(str);
  // Serial.print("Checksum (msg): ");
  // Serial.println((int)csum);
  // Serial.print("Checksum (calc): ");
  // Serial.println((int)xor_check(&m_bfr[1]));

  if (csum != (uint16_t)xor_check(&m_bfr[1]))
    return false;

  return true;
}

bool 
CommandProtocol::sendString(char* msg)
{
  if (sprintf(m_bfr, "$%s%02x\n", msg, xor_check(msg)) < 0)
    return false;

  SerialUtils::sendMessage(m_bfr);
  return true;
}

bool
CommandProtocol::decode(Command& cmd)
{
  //Clear command
  cmd.reset();

  uint8_t filled_args;
  filled_args = sscanf(m_msg, "%c,%c,%u,%u,%u", &cmd.cmd_type,
                                                &cmd.dev,
                                                &cmd.dev_num,
                                                &cmd.val[0],
                                                &cmd.val[1]);
  
  if (filled_args < 3)
    return false;
  
  if (cmd.cmd_type == CMD_TYPE_GET)
  {
    if (filled_args != 3)
      return false;
  }
  else
  {
    if (cmd.dev == DEV_MOTOR)
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
CommandProtocol::encode(Command& cmd)
{
  uint8_t lenght;
  lenght = sprintf(m_msg, "%c,%c,%u", cmd.cmd_type,
                                       cmd.dev,
                                       cmd.dev_num);

  if (lenght < 0)                          
    return false;

  for (uint8_t i = 0; i < 2; i++)
  {
    if (cmd.val[i] != 65535) // Invalid (uint16_t)(-1)
      lenght += sprintf(&m_msg[lenght], ",%u", cmd.val[i]);
  }
  sprintf(&m_msg[lenght], "%c", '*');

  return true;
}

bool 
CommandProtocol::receiveCommand(Command& cmd)
{
  if (!receiveString(m_msg))
    return false;

  if (!decode(cmd))
    return false;

  return true;
}

bool 
CommandProtocol::sendCommand(Command& cmd)
{
  //Should only send CMD_TYPE_INFO
  if (cmd.cmd_type != CMD_TYPE_INFO)
    return false;
  
  if (!encode(cmd))
    return false;

  return sendString(m_msg);
}

bool
CommandProtocol::sendOk()
{
  char msg[] = "OK*";
  return sendString(msg);
}

bool
CommandProtocol::sendError(char* error_msg)
{
  char msg[CP_BUFFER_SIZE];
  sprintf(msg, "ERROR,%s*", error_msg);
  return sendString(msg);
}

bool
CommandProtocol::sendHeartbeat()
{
  char msg[] = "HB*";
  return sendString(msg);
}