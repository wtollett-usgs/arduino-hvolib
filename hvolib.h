#ifndef hvolib_h
#define hvolib_h

#include "Arduino.h"
#include <DHT.h>
#include <hvorest.h>

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

    static HVORest* getRest() {
      static HVORest rest_;
      return &rest_;
    }

    void setup(uint8_t n48v, uint8_t nCurrent, uint8_t nVolt, uint8_t nDHTs);

    void add48vPin(uint8_t id, uint8_t pin);
    void addCurrentPin(uint8_t id, uint8_t pin);
    void addVoltagePin(uint8_t id, uint8_t pin);
    void addDHTDevice(uint8_t id, uint8_t ptr);

    static uint8_t getAnalog(uint8_t pinNum);
    static uint8_t getDigital(uint8_t pinNum);

    static uint8_t getAllSensors(uint8_t id);
    static uint8_t get48v(uint8_t id);
    static uint8_t getAmps(uint8_t id);
    static uint8_t getCurrent(uint8_t id);
    static uint8_t getHumidity(uint8_t id);
    static uint8_t getTemp(uint8_t id);
    static uint8_t getVoltage(uint8_t id);

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

    static bool startsWith(const char* str, const char* pre);
};

#endif
