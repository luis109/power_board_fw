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

  if (cmd.m_dev_num != 65535) // Invalid (uint16_t)(-1)
    sentence.addField(cmd.m_dev_num);

  for (uint8_t i = 0; i < 2; i++)
  {
    if (cmd.m_val[i] != 65535) // Invalid (uint16_t)(-1)
      sentence.addField(cmd.m_val[i]);
  }
}

bool 
CommandProtocol::receiveCommand(Command& cmd)
{
  char bfr[CP_BUFFER_SIZE];
  uint8_t length = SerialUtils::receiveMessage(bfr, CP_BUFFER_SIZE);

  if (length == 0)
    return false;

  NMEASentence sentence(bfr);
  if (!decode(cmd, sentence))
    return false;

  // Serial.print("Type: ");Serial.println(cmd.m_type);
  // Serial.print("Dev: ");Serial.println(cmd.m_dev);
  // Serial.print("Dev Num: ");Serial.println(cmd.m_dev_num);
  // Serial.print("Val 0: ");Serial.println(cmd.m_val[0]);
  // Serial.print("Val 1: ");Serial.println(cmd.m_val[1]);

  return true;
}

bool 
CommandProtocol::sendCommand(Command& cmd)
{
  //Should only send CMD_TYPE_INFO
  if (cmd.m_type != CMD_TYPE_INFO && cmd.m_type != CMD_TYPE_VERSION)
    return false;
  
  NMEASentence sentence;
  encode(cmd, sentence);

  char bfr[CP_BUFFER_SIZE];
  sentence.getSentence(bfr);
  return SerialUtils::sendMessage(bfr) > 0;
}

bool
CommandProtocol::sendOk()
{
  NMEASentence sentence;
  char bfr[CP_BUFFER_SIZE];

  sentence.addField("OK");
  sentence.getSentence(bfr);

  return SerialUtils::sendMessage(bfr) > 0;
}

bool
CommandProtocol::sendError(const char* error_msg)
{
  NMEASentence sentence;
  char bfr[CP_BUFFER_SIZE];
  
  sprintf(bfr, "ERROR,%s", error_msg);
  sentence.addField(bfr);
  sentence.getSentence(bfr);

  return SerialUtils::sendMessage(bfr) > 0;
}