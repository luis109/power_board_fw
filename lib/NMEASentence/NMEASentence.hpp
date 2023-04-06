#ifndef NMEA_SENTENCE_HPP
#define NMEA_SENTENCE_HPP

#include <SerialUtils.hpp>

#define NMEA_MAX_FIELDS 8
#define NMEA_MAX_FIELD_LENGTH 32
#define NMEA_MAX_LENGTH() NMEA_MAX_FIELDS * NMEA_MAX_FIELD_LENGTH

typedef char NMEAFields[NMEA_MAX_FIELDS][NMEA_MAX_FIELD_LENGTH];

class NMEASentence
{
  public:
    //! Empty initializer
    NMEASentence();

    //! Initialize with sentence
    NMEASentence(const char* sentence);

    //! Initialize with fields
    NMEASentence(const NMEAFields fields);

    //! Reset NMEA sentence
    void
    reset();

    //! Set NMEA sentence and get fields
    bool
    setSentence(const char* sentence);

    //! Add field and make sentence
    void
    addField(const char* field);

    //! Add numeric field and make sentence
    void
    addField(const char field);

    //! Add numeric field and make sentence
    void
    addField(const uint16_t field);

    //! Get field
    bool
    getField(uint8_t index, char* field);

    uint8_t
    getMaxFields()
    {
      return m_field_index;
    }

    //! Get NMEA sentence
    void
    getSentence(char* sentence);

    //! Check if sentence is valid
    //! @param sentence sentence to check
    //! @return true if valid, false otherwise
    static bool
    checkSentence(const char* sentence);

  private:
    //! NMEA sentence field array
    NMEAFields m_fields;
    //! Field index
    uint8_t m_field_index;
    //! Full NMEA sentence
    char m_sentence[NMEA_MAX_LENGTH()];

    //! Fill in fields from sentence
    void
    makeFields();

    //! Fill in fields from sentence
    void
    makeSentence();

};

#endif