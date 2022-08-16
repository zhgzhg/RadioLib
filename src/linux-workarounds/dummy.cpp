#ifdef LINUX
  #include <cstdio>
  #include "TypeDef.h"
  #include "SPI.h"

  void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode) { 
    printf("not implemented %s\n", __func__);
  }

  void detachInterrupt(uint8_t interruptNum) {
    printf("not implemented %s\n", __func__);
  }

  unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout_unused) {

    const unsigned int maxLoops = 1000000;
    unsigned int numloops = 0;

    while (digitalRead(pin) == state)
    {
      if (numloops++ == maxLoops)
        return 0 ;
    }

    while (digitalRead(pin) != state)
    {
      if (numloops++ == maxLoops)
        return 0 ;
    }

    int timerStart = micros();
    while (digitalRead(pin) == state)
    {
      if (numloops++ == maxLoops)
        return 0 ;
    }

    return micros() - timerStart ;
  }

  _BIN BIN;
  MockDebugSerial Serial, Serial1;

  #ifdef NOWIRINGIPI
    void digitalWrite(RADIOLIB_PIN_TYPE pin, RADIOLIB_PIN_STATUS value) { }
    RADIOLIB_PIN_STATUS digitalRead(RADIOLIB_PIN_TYPE pin) {
      return 0;
    }
    void pinMode(RADIOLIB_PIN_TYPE pin, RADIOLIB_PIN_MODE mode) { }
    void yield() { }
    int wiringPiSetup() { return 0; }
    int wiringPiSPISetup(int channel, int speed) { return 0; }
    int wiringPiSPISetupMode(int channel, int speed, int mode) { return -1; }
    int wiringPiSPISetupModePort(int channel, int speed, int mode, int port) { return -1; }
    int wiringPiSPIDataRW(int channel, unsigned char *data, int len) { return -1; }
    unsigned int millis() { return 0; }
    unsigned int micros() { return 0; }
    void delay(unsigned int howLong) { }
    void delayMicroseconds(unsigned int howLong) { }
  #else
    void yield() {
      delayMicroseconds(100);
    }
  #endif

#endif
