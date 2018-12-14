#include "hvolib.h"

void HVOLib::setup(uint8_t n48v, uint8_t nCurrent, uint8_t nVolt, uint8_t nDHTs) {
  n48v_ = n48v;
  nCurrent_ = nCurrent;
  nVolt_ = nVolt;
  nDHTs_ = nDHTs;

  FEvPins_ = (Pins*)malloc(n48v * sizeof(Pins));
  currentPins_ = (Pins*)malloc(nCurrent * sizeof(Pins));
  voltPins_ = (Pins*)malloc(nVolt * sizeof(Pins));
  dhtDevices_ = (DHTs*)malloc(nDHTs * sizeof(DHTs));

  FEvIndex_ = 0;
  currentIndex_ = 0;
  voltIndex_ = 0;
  dhtIndex_ = 0;
}

void HVOLib::add48vPin(uint8_t id, uint8_t pin) {
  FEvPins_[FEvIndex_].id = id;
  FEvPins_[FEvIndex_++].pin = pin;
}

void HVOLib::addCurrentPin(uint8_t id, uint8_t pin) {
  currentPins_[currentIndex_].id = id;
  currentPins_[currentIndex_++].pin = pin;
}

void HVOLib::addDHTDevice(uint8_t id, uint8_t pin) {
  dhtDevices_[dhtIndex_].id = id;
  DHT dht;
  dht.setup(pin, DHT::DHT22);
  dhtDevices_[dhtIndex_++].dhtInstance = dht;
}

void HVOLib::addVoltagePin(uint8_t id, uint8_t pin) {
  voltPins_[voltIndex_].id = id;
  voltPins_[voltIndex_++].pin = pin;
}

static uint8_t HVOLib::get48v(String command) {
  char str[10];
  uint8_t id = atoi(command.c_str());

  uint8_t pin = getInstance()->get48vPin(id);
  int val = analogRead(pin);
  float temp = (val * 5.89035e-02) + 5.39413e-02;

  getRest()->addToBuffer(F("\"48v"));
  getRest()->addToBuffer(id);
  getRest()->addToBuffer(F("\": "));
  ftoa(str, temp, 2);
  getRest()-> addToBuffer(str);
  getRest()->addToBuffer(F(", "));

  return val;
}

static uint8_t HVOLib::getAllSensors(String command) {
  // Temp
  for (uint8_t i = 0; i < getInstance()->nDHTs_; i++) {
    getTemp(String(i+1));
  }

  // 48v
  for (uint8_t i = 0; i < getInstance()->n48v_; i++) {
    get48v(String(i+1));
  }

  // Amps
  for (uint8_t i = 0; i < getInstance()->nCurrent_; i++) {
    getAmps(String(i+1));
  }

  // Voltage
  for (uint8_t i = 0; i < getInstance()->nVolt_; i++) {
    getVoltage(String(i+1));
  }

  // Temp
  for (uint8_t i = 0; i < getInstance()->nDHTs_; i++) {
    getHumidity(String(i+1));
  }
}

static uint8_t HVOLib::getAmps(String command) {
  char str[10];
  uint8_t id = atoi(command.c_str());

  uint8_t pin = getInstance()->getCurrentPin(id);
  float v = 0.0;
  for (uint16_t i = 0; i < 500; i++) {
    v += (((.0049 * analogRead(pin)) - 2.5) / .066);
    delay(1);
  }
  v = v / 500;

  getRest()->addToBuffer(F("\"amps"));
  getRest()->addToBuffer(id);
  getRest()->addToBuffer(F("\": "));
  ftoa(str, v, 3);
  getRest()->addToBuffer(str);
  getRest()->addToBuffer(F(", "));

  return pin;
}

static uint8_t HVOLib::getCurrent(String command) {
  char str[10];
  uint8_t id = atoi(command.c_str());

  uint8_t pin = getInstance()->getCurrentPin(id);
  int val = analogRead(pin);
  float v = val * (5.0 / 1023.0);

  getRest()->addToBuffer(F("\"current"));
  getRest()->addToBuffer(id);
  getRest()->addToBuffer(F("\": "));
  ftoa(str, v, 3);
  getRest()->addToBuffer(str);
  getRest()->addToBuffer(F(", "));

  return val;
}

static uint8_t HVOLib::getHumidity(String command) {
  char str[10];
  uint8_t id = atoi(command.c_str());

  getRest()->addToBuffer(F("\"humidity"));
  getRest()->addToBuffer(id);
  getRest()->addToBuffer(F("\": "));

  DHT d = getInstance()->getDHTDevice(id);
  float val = d.getHumidity();
  ftoa(str, val, 2);
  getRest()->addToBuffer(str);
  getRest()->addToBuffer(F(", "));

  return 1;
}

static uint8_t HVOLib::getTemp(String command) {
  char str[10];
  uint8_t id = atoi(command.c_str());

  getRest()->addToBuffer(F("\"temp"));
  getRest()->addToBuffer(id);
  getRest()->addToBuffer(F("\": "));

  DHT d = getInstance()->getDHTDevice(id);
  delay(d.getMinimumSamplingPeriod());
  ftoa(str, d.getTemperature(), 2);

  getRest()->addToBuffer(str);
  getRest()->addToBuffer(F(", "));

  return 1;
}

static uint8_t HVOLib::getVoltage(String command) {
  char str[10];
  uint8_t id = atoi(command.c_str());

  uint8_t pin = getInstance()->getVoltagePin(id);
  int val = analogRead(pin);
  float temp = val / 4.092;
  temp = fmod(temp, 1000);

  getRest()->addToBuffer(F("\"voltage"));
  getRest()->addToBuffer(id);
  getRest()->addToBuffer(F("\": "));
  ftoa(str, temp/10, 2);
  getRest()->addToBuffer(str);
  getRest()->addToBuffer(F(", "));

  return val;
}

uint8_t HVOLib::get48vPin(uint8_t v) {
  return getPin(v, n48v_, FEvPins_);
}

uint8_t HVOLib::getCurrentPin(uint8_t v) {
  return getPin(v, nCurrent_, currentPins_);
}

uint8_t HVOLib::getVoltagePin(uint8_t v) {
  return getPin(v, nVolt_, voltPins_);
}

uint8_t HVOLib::getPin(uint8_t id, uint8_t sz, Pins* arr) {
  if (sz == 1) {
    return arr[0].pin;
  }

  uint8_t ret = 0;

  for (uint8_t i = 0; i < sz; i++) {
    if (id == arr[i].id) {
      ret = arr[i].pin;
      break;
    }
  }

  return ret;
}

DHT HVOLib::getDHTDevice(uint8_t v) {
  if (nDHTs_ == 1) {
    return dhtDevices_[0].dhtInstance;
  }

  for (uint8_t i = 0; i < nDHTs_; i++) {
    if (v == dhtDevices_[i].id) {
      return dhtDevices_[i].dhtInstance;
    }
  }
}

static char* HVOLib::ftoa(char* a, double f, uint8_t precision) {
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};

  char* ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
}

static bool HVOLib::startsWith(const char* str, const char* pre) {
  size_t lenpre = strlen(pre), lenstr = strlen(str);
  return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}
