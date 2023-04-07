#include <CommandProtocol.hpp>

// #define _COMMAND_PROTOCOL_DEBUG

void
CommandProtocol::begin(int baud)
{
  SerialUtils::begin(baud);
}

bool
CommandProtocol::decode(Command& cmd, NMEASentence& sentence)
{
  //Clear command
  cmd.reset();
  char bfr[16];

  // Command must have at least a type
  if (!sentence.getField(0, &cmd.m_type))
    return false;

  if (!sentence.getField(1, &cmd.m_dev))
    return true;

  if (!sentence.getField(2, bfr))
    return true;
  cmd.m_dev_num = atoi(bfr);

  if (!sentence.getField(3, bfr))
    return true;
  cmd.m_val[0] = atoi(bfr);

  if (!sentence.getField(4, bfr))
    return true;
  cmd.m_val[1] = atoi(bfr);

  return true;
}

void
CommandProtocol::encode(Command& cmd, NMEASentence& sentence)
{
  sentence.addField(cmd.m_type);
  sentence.addField(cmd.m_dev);

  if (cmd.m_dev_num != (uint16_t)(-1)) // Invalid (uint16_t)(-1)
    sentence.addField(cmd.m_dev_num);

  for (uint8_t i = 0; i < 2; i++)
  {
    if (cmd.m_val[i] != (uint16_t)(-1)) // Invalid (uint16_t)(-1)
      sentence.addField(cmd.m_val[i]);
  }
}

bool 
CommandProtocol::receiveCommand(Command& cmd)
{
  uint8_t length = SerialUtils::receiveMessage(m_bfr, NMEA_MAX_LENGTH());

  if (length == 0)
    return false;

  NMEASentence sentence(m_bfr);
  if (!decode(cmd, sentence))
    return false;

#ifdef _COMMAND_PROTOCOL_DEBUG
  Serial.print("Type: ");Serial.println(cmd.m_type);
  Serial.print("Dev: ");Serial.println(cmd.m_dev);
  Serial.print("Dev Num: ");Serial.println(cmd.m_dev_num);
  Serial.print("Val 0: ");Serial.println(cmd.m_val[0]);
  Serial.print("Val 1: ");Serial.println(cmd.m_val[1]);
#endif

  return true;
}

bool 
CommandProtocol::sendCommand(Command& cmd)
{
  NMEASentence sentence;
  encode(cmd, sentence);

  sentence.getSentence(m_bfr);
  return SerialUtils::sendMessage(m_bfr) > 0;
}

bool
CommandProtocol::sendAck()
{
  NMEASentence sentence;

  sentence.addField("ACK");
  sentence.getSentence(m_bfr);

  return SerialUtils::sendMessage(m_bfr) > 0;
}

bool
CommandProtocol::sendError(const char* error_msg)
{
  NMEASentence sentence;
  
  sprintf(m_bfr, "ERROR,%s", error_msg);
  sentence.addField(m_bfr);
  sentence.getSentence(m_bfr);

  return SerialUtils::sendMessage(m_bfr) > 0;
}