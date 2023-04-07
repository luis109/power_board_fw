#include <NMEASentence.hpp>

// #define _NMEA_SENTENCE_DEBUG

uint8_t 
xor_check(char *data)
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

NMEASentence::NMEASentence()
{
  reset();
}

NMEASentence::NMEASentence(const char* sentence):
  NMEASentence()
{
  if (!checkSentence(sentence))
    return;

  strcpy(m_sentence, sentence);
  makeFields();
}

NMEASentence::NMEASentence(const NMEAFields fields):
  NMEASentence()
{
  for (uint8_t i = 0; i < NMEA_MAX_FIELDS; i++)
    strcpy(m_fields[i], fields[i]);
  makeSentence();
}

void
NMEASentence::reset()
{
  m_field_index = 0;

  memset(m_sentence,'\0', sizeof(NMEA_MAX_LENGTH()));

  for (uint8_t i = 0; i < NMEA_MAX_FIELDS; i++)
    memset(m_fields[i],'\0', sizeof(NMEA_MAX_FIELD_LENGTH));
}

bool
NMEASentence::checkSentence(const char* sentence)
{
  if (sentence[0] == '\0')
    return false;
  
  if (sentence[0] != '$')
    return false;

  uint16_t csum;
  char str[NMEA_MAX_LENGTH()];
  sscanf(&sentence[1], "%[^*]*%x%*s", str, &csum);

  if (csum != (uint16_t)xor_check(str))
    return false;

  return true;
}

void
NMEASentence::makeFields()
{
  char str[NMEA_MAX_LENGTH()];
  const char sep = ',';
  char *token;

  // Strip '$' and checksum
  sscanf(&m_sentence[1], "%[^*]*%*s", str);

  // Restart index
  m_field_index = 0;

  // Seperate fields
  token = strtok(str, &sep);
  while( token != NULL ) 
  {
    strcpy(m_fields[m_field_index], token);
    m_field_index++;
    token = strtok(NULL, &sep);

    if (*token == '\0')
      break;
  }

#ifdef _NMEA_SENTENCE_DEBUG
  for (uint8_t i = 0; i < m_field_index; i++)
  {
    Serial.print("Field ");Serial.print(i);Serial.print(": ");
    Serial.println(m_fields[i]);
  }
#endif
}

void
NMEASentence::makeSentence()
{
  // Build sentence with fields only
  char str[NMEA_MAX_LENGTH()];
  uint8_t lenght;

  lenght = sprintf(str, "%s", m_fields[0]);
  for (uint8_t i = 1; i < m_field_index; i++)
    lenght += sprintf(&str[lenght], ",%s", m_fields[i]);

  sprintf(m_sentence, "$%s*%02x\n", str, xor_check(str));

#ifdef _NMEA_SENTENCE_DEBUG
  Serial.print("Sentence: ");Serial.println(m_sentence);
#endif
}

bool
NMEASentence::setSentence(const char* sentence)
{
  if (!checkSentence(sentence))
    return false;

  strcpy(m_sentence, sentence);
  makeFields();
  return true;
}

void
NMEASentence::addField(const char* field)
{
  sprintf(m_fields[m_field_index], "%s", field);
  m_field_index++;

  makeSentence();
}

void
NMEASentence::addField(const char field)
{
  sprintf(m_fields[m_field_index], "%c", field);
  m_field_index++;

  makeSentence();
}

void
NMEASentence::addField(const uint16_t field)
{
  sprintf(m_fields[m_field_index], "%u", field);
  m_field_index++;

  makeSentence();
}

bool
NMEASentence::getField(uint8_t index, char* field)
{
  if (index >= m_field_index)
    return false;

  strcpy(field, m_fields[index]);
  return true;
}

void
NMEASentence::getSentence(char* sentence)
{
  strcpy(sentence, m_sentence);
}
