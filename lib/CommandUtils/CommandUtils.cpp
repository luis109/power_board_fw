#include <CommandUtils.hpp>

uint8_t xor_check(char *data)
{
  uint8_t csum = 0x00;
  uint8_t t = 0;
  while (data[t] != DELIM_TERMINATOR)
  {
    csum ^= data[t];
    t++;
  }
  return csum;
}

void
CommandUtils::begin(int baud)
{
  SerialUtils::begin(baud);
}

bool 
CommandUtils::receiveCommand(Command& cmd)
{
  uint8_t length = SerialUtils::receiveMessage(m_bfr, CMDUTIL_BUFFER_SIZE);

  if (length == 0)
    return false;
  
  if (m_bfr[0] != DELIM_INITIALIZER)
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
CommandUtils::decode(Command& cmd)
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
CommandUtils::encode(Command& cmd)
{
  if (cmd.dev == DEV_MOTOR)
  {
    uint8_t size;
    size = sprintf(m_msg, "%c,%u,%c,%u,%u*", cmd.dev,
                                             cmd.dev_num,
                                             cmd.cmd_type,
                                             cmd.val[0],
                                             cmd.val[1]);

    if (size < 0)                                                  
      return false;
  }
  else if (cmd.dev == DEV_PWM || cmd.dev == DEV_RELAY)
  {
    uint8_t size;
    size = sprintf(m_msg, "%c,%u,%c,%u*", cmd.dev,
                                          cmd.dev_num,
                                          cmd.cmd_type,
                                          cmd.val[0]);
    
    if (size < 0)
      return false;
  }
  else
    return false;

  Serial.print("Message: ");Serial.println(m_msg);
  return true;
}

bool 
CommandUtils::sendCommand(Command& cmd)
{
  //Should only send CMD_TYPE_INFO
  if (cmd.cmd_type != CMD_TYPE_INFO)
    return false;
    
  if (!encode(cmd))
    return false;

  if (sprintf(m_bfr, "$%s%x\n", m_msg, xor_check(m_msg)) < 0)
    return false;

  SerialUtils::sendMessage(m_bfr);

  return true;
}

bool
CommandUtils::sendReply(uint8_t rply)
{
  if (rply == RPLY_OK)
    sprintf(m_msg, "OK*");
  else if (rply == RPLY_FAILED)
    sprintf(m_msg, "FAILED*");

  if (sprintf(m_bfr, "$%s%x", m_msg, xor_check(m_msg)) < 0)
    return false;

  SerialUtils::sendMessage(m_bfr);
  return true;
}

bool
CommandUtils::sendHeartbeat()
{
  if (sprintf(m_bfr, "$HB*0a") < 0)
    return false;

  SerialUtils::sendMessage(m_bfr);
  return true;
}