#ifndef hvolib_h
#define hvolib_h

#include "Arduino.h"
#include <DHT.h>
#include <aREST.h>

struct dhts {
  uint8_t id;
  DHT dhtInstance;
};

struct pins {
  uint8_t id;
  uint8_t pin;
};

typedef struct dhts DHTs;
typedef struct pins Pins;

class HVOLib {
  public:
    static HVOLib* getInstance() {
      static HVOLib instance;
      return &instance;
    }

    static aREST* getRest() {
      static aREST rest_;
      return &rest_;
    }

    void setup(uint8_t n48v, uint8_t nCurrent, uint8_t nVolt, uint8_t nDHTs);

    void add48vPin(uint8_t id, uint8_t pin);
    void addCurrentPin(uint8_t id, uint8_t pin);
    void addVoltagePin(uint8_t id, uint8_t pin);
    void addDHTDevice(uint8_t id, uint8_t ptr);

    static uint8_t getAllSensors(String command);
    static uint8_t get48v(String command);
    static uint8_t getAmps(String command);
    static uint8_t getCurrent(String command);
    static uint8_t getHumidity(String command);
    static uint8_t getTemp(String commnad);
    static uint8_t getVoltage(String command);

  private:
    uint8_t n48v_;
    uint8_t nCurrent_;
    uint8_t nVolt_;
    uint8_t nDHTs_;

    Pins* FEvPins_;
    Pins* currentPins_;
    Pins* voltPins_;
    DHTs* dhtDevices_;

    uint8_t FEvIndex_;
    uint8_t currentIndex_;
    uint8_t voltIndex_;
    uint8_t dhtIndex_;

    uint8_t get48vPin(uint8_t v);
    uint8_t getCurrentPin(uint8_t v);
    uint8_t getVoltagePin(uint8_t v);

    HVOLib() {};

    uint8_t getPin(uint8_t id, uint8_t sz, Pins* arr);
    DHT getDHTDevice(uint8_t v);

    static char* ftoa(char* a, double f, uint8_t precision);
    static bool startsWith(const char* str, const char* pre);
};

#endif
