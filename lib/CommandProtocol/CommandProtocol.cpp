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
CommandProtocol::receiveCommand(Command& cmd)
{
  uint8_t length = SerialUtils::receiveMessage(m_bfr, CP_BUFFER_SIZE);

  if (length == 0)
    return false;
  
  if (m_bfr[0] != '$')
    return false;

  uint16_t csum;
  sscanf(&m_bfr[1], "%[^*]*%x%*s", m_msg, &csum);

  // Serial.print("Message: ");
  // Serial.println(m_msg);
  // Serial.print("Checksum (msg): ");
  // Serial.println((int)csum);
  // Serial.print("Checksum (calc): ");
  // Serial.println((int)xor_check(&m_bfr[1]));

  if (csum != (uint16_t)xor_check(&m_bfr[1]))
    return false;

  if (!decode(cmd))
    return false;

  return true;
}

bool
CommandProtocol::decode(Command& cmd)
{
  //Clear command
  cmd.reset();

  // sscanf cant assignt to uint8_t, minimum is uint16_t, 
  // so we must scan to temporary uint16_t variables and 
  // convert after. This sets values to invalid.
  uint16_t aux[] = {255, 255, 255};

  if (m_msg[0] == DEV_MOTOR)
  {
    uint8_t filled_args;
    filled_args = sscanf(m_msg, "%c,%u,%c,%u,%u", &cmd.dev,
                                                  &aux[0],
                                                  &cmd.cmd_type,
                                                  &aux[1],
                                                  &aux[2]);

    if (filled_args != 3 && filled_args != 5)// GET command
      return false;
  }
  else if (m_msg[0] == DEV_PWM || m_msg[0] == DEV_RELAY)
  {
    uint8_t filled_args;
    filled_args = sscanf(m_msg, "%c,%u,%c,%u", &cmd.dev,
                                               &aux[0],
                                               &cmd.cmd_type,
                                               &aux[1]);

    if (filled_args != 3 && filled_args != 4)// GET command
      return false;
  }
  else
    return false;

  cmd.typeConvert(aux);
  // Serial.print("Dev: ");Serial.println(cmd.dev);
  // Serial.print("Dev_num: ");Serial.println(cmd.dev_num);
  // Serial.print("Cmd_type: ");Serial.println(cmd.cmd_type);
  // Serial.print("Val[0]: ");Serial.println(cmd.val[0]);
  // Serial.print("Val[1]: ");Serial.println(cmd.val[1]);
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
CommandProtocol::sendString(char* msg)
{
  if (sprintf(m_bfr, "$%s%02x\n", msg, xor_check(msg)) < 0)
    return false;

  SerialUtils::sendMessage(m_bfr);
  return true;
}

bool
CommandProtocol::sendOk()
{
  char* msg = "OK*";
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
  char* msg = "HB*";
  return sendString(msg);
}