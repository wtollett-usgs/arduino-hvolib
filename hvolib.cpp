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

static uint8_t HVOLib::getAnalog(uint8_t pinNum) {
  getRest()->addData("Value", analogRead(pinNum));
  getRest()->addData("PinNumber", pinNum);
}

static uint8_t HVOLib::getDigital(uint8_t pinNum) {
  getRest()->addData("Value", digitalRead(pinNum));
  getRest()->addData("PinNumber", pinNum);
}

static uint8_t HVOLib::get48v(uint8_t id) {
  char str[10] = { 0 };

  uint8_t pin = getInstance()->get48vPin(id);
  int val = analogRead(pin);
  float temp = (val * 5.89035e-02) + 5.39413e-02;
  sprintf(str, "48v%d", id);
  getRest()->addData(str, temp, 2);

  return val;
}

static uint8_t HVOLib::getAllSensors(uint8_t id) {
  // Temp & Humidity
  for (uint8_t i = 0; i < getInstance()->nDHTs_; i++) {
    getTemp(i+1);
    getHumidity(i+1);
  }

  // Given enough DHT sensors, these could take so much
  // time that a watchdog reset is necessary in order to
  // keep everything running.
  wdt_reset();

  // 48v
  for (uint8_t i = 0; i < getInstance()->n48v_; i++) {
    get48v(i+1);
  }

  // Amps
  for (uint8_t i = 0; i < getInstance()->nCurrent_; i++) {
    getAmps(i+1);
  }

  // Voltage
  for (uint8_t i = 0; i < getInstance()->nVolt_; i++) {
    getVoltage(i+1);
  }

}

static uint8_t HVOLib::getAmps(uint8_t id) {
  char str[10] = { 0 };

  uint8_t pin = getInstance()->getCurrentPin(id);
  float v = 0.0;
  for (uint16_t i = 0; i < 500; i++) {
    v += (((.0049 * analogRead(pin)) - 2.5) / .066);
    delay(1);
  }
  v = v / 500;

  sprintf(str, "amps%d", id);
  getRest()->addData(str, v, 3);

  return pin;
}

static uint8_t HVOLib::getCurrent(uint8_t id) {
  char str[10] = { 0 };

  uint8_t pin = getInstance()->getCurrentPin(id);
  int val = analogRead(pin);
  float v = val * (5.0 / 1023.0);

  sprintf(str, "current%d", id);
  getRest()->addData(str, v, 3);

  return val;
}

static uint8_t HVOLib::getHumidity(uint8_t id) {
  char str[10] = { 0 };

  DHT d = getInstance()->getDHTDevice(id);
  delay(d.getMinimumSamplingPeriod());
  sprintf(str, "humidity%d", id);
  getRest()->addData(str, d.getHumidity(), 2);

  return 1;
}

static uint8_t HVOLib::getTemp(uint8_t id) {
  char str[10] = { 0 };

  DHT d = getInstance()->getDHTDevice(id);
  delay(d.getMinimumSamplingPeriod());
  sprintf(str, "temp%d", id);
  getRest()->addData(str, d.getTemperature(), 2);

  return 1;
}

static uint8_t HVOLib::getVoltage(uint8_t id) {
  char str[10] = { 0 };

  uint8_t pin = getInstance()->getVoltagePin(id);
  int val = analogRead(pin);
  float temp = val / 4.092;
  temp = fmod(temp, 1000);
  sprintf(str, "voltage%d", id);
  getRest()->addData(str, temp/10, 2);

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

static bool HVOLib::startsWith(const char* str, const char* pre) {
  size_t lenpre = strlen(pre), lenstr = strlen(str);
  return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}
