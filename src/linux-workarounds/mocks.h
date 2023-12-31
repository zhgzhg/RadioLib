#ifdef LINUX
  #include <inttypes.h>
  #include <stdint.h>
  #include <stddef.h>
  #include <cstring>
  #include <cmath>

  #include <string>
  #include <iostream>
  #include <iomanip>
  #include <limits>
  #include <locale>
  #include <type_traits>

  #define String std::string

  #define CHANGE 1
  #define FALLING 2
  #define RISING 3

  void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode);
  void detachInterrupt(uint8_t interruptNum);
  /*
   * pulseIn:
   *  Reads a pulse (either HIGH or LOW) on a pin.
   *  Return the length of the pulse in microseconds.
   *  Gives up and returns 0 if no pulse starts.
   *  Works on pulses from 100 microseconds to 3000 microseconds in length (need more tests)
   *********************************************************************************
   */
  unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout_unused = 0);

  #ifndef DUMMY_ARDUINO_FUNCS
    #define DUMMY_ARDUINO_FUNCS
    #define digitalPinToInterrupt(p) ({ p;})
    #define max(p, n) std::max(p, n)
    #define ceil(p) std::ceil(p)
    #define my_typeof(x) std::remove_reference<decltype((x))>::type

    class __FlashStringHelper;
    #define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
    #define F(string_literal) (FPSTR(PSTR(string_literal)))
    //#define F(string_literal) (string_literal)
    #define ICACHE_RODATA_ATTR
    #define PROGMEM ICACHE_RODATA_ATTR
    #define PGM_P const char *
    #define PGM_VOID_P const void *
    #define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))
    #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
    #define pgm_read_dword(addr) ({ \
      my_typeof(addr) _addr = (addr); \
      *(const unsigned long *)(_addr); \
    })

    void yield();
    typedef std::ios_base& (*mock_manipulator)(std::ios_base&);

    #ifndef HEX
      #define HEX 16
    #endif
    #ifndef DEC
      #define DEC 10
    #endif
    #ifndef OCT
      #define OCT 8
    #endif

    #define is_char_type(T) ( std::is_same<T, char>::value || \
      std::is_same<T, unsigned char>::value || std::is_same<T, uint8_t>::value || \
      std::is_same<T, int8_t>::value || std::is_same<T, wchar_t>::value || \
      std::is_same<T, char16_t>::value || std::is_same<T, char32_t>::value )

    template <typename _T>
    struct _binary { std::string _digits; };

    template <typename T>
    constexpr auto _mypow(T b, unsigned int e ) -> decltype(b + 1)
    { return e ? b * _mypow( b, e - 1 ) : 1 ; }

    template <typename T>
    inline _binary<T> binary(T n) {
      _binary<T> __binary;
      constexpr int bits = _mypow(8U, sizeof(n)) - 1;

      for (int i = bits; i >= 0; --i) {
        if ((n >> i) & 1) __binary._digits.append(1, '1');
        else __binary._digits.append(1, '0');
      }

      return __binary;
    }

    template <typename T>
    inline std::ostream& operator<<(std::ostream& stream, _binary<T> __binary) {
      stream << __binary._digits;
      return stream;
    }

    struct _BIN {};

    class countbuf : public std::streambuf {
      std::streambuf* sbuf;
      std::streamsize size;
    public:
      countbuf(std::streambuf* sbuf): sbuf(sbuf), size() {}
      int overflow(int c) {
        if (traits_type::eof() != c) {
          ++this->size;
        }
        return this->sbuf->sputc(c);
      }
      int sync() { return this->sbuf->pubsync(); }
      std::streamsize count() { return this->size; }
      std::streambuf* wrapped() { return sbuf; }
    };

    class MockPrint { };
    class MockPrintable {
    public:
      virtual size_t printTo(MockPrint& p) const = 0;
    };

    class MockDebugSerial {
    public:
      operator bool() { return true; }

      template <typename SPD, typename CFG=int>
      void begin(SPD speed, CFG config={}) { }
      void end() { }

      int peek() { return 0; }
      int read() { return 0; }
      int available() { return 0; }
      void flush() { }

      size_t print() { return 0; }
      size_t println() { std::cerr << std::endl; return 1; }

      template <typename NI>
      size_t print(NI number, mock_manipulator mm) {
        static_assert(std::is_integral<NI>::value, "Integral required.");
        typename std::conditional<is_char_type(NI), unsigned long, NI>::type _number = number;

        countbuf sbuf(std::cerr.rdbuf());
        std::streambuf* orig = std::cerr.rdbuf(&sbuf);
        std::cerr << mm << _number;
        std::cerr.rdbuf(orig);
        return sbuf.count();
      }
      template <typename NI>
      size_t println(NI number, mock_manipulator mm) {
        static_assert(std::is_integral<NI>::value, "Integral required.");
        return print(number, mm) + println();
      }

      template <typename NI>
      size_t print(NI number, struct _BIN mm) {
        static_assert(std::is_integral<NI>::value, "Integral required.");

        countbuf sbuf(std::cerr.rdbuf());
        std::streambuf* orig = std::cerr.rdbuf(&sbuf);
        std::cerr << binary(number);
        std::cerr.rdbuf(orig);
        return sbuf.count();
      }
      template <typename NI>
      size_t println(NI number, struct _BIN mm) {
        static_assert(std::is_integral<NI>::value, "Integral required.");
        return print(number, mm) + println();
      }
      template <typename NI>
      std::enable_if_t<std::is_integral<NI>::value, size_t> print(NI number, int base) {
        switch(base) {
          case(2): {
            _BIN b;
            return print(number, b);
          }
          case(8): return print(number, std::oct);
          case(10): return print(number, std::dec);
          case(16): return print(number, std::hex);
          default: return print(number, std::dec);
        }
      }

      template <typename ND>
      std::enable_if_t<std::is_floating_point<ND>::value, size_t> print(ND number, int precision) {
        countbuf sbuf(std::cerr.rdbuf());
        std::streambuf* orig = std::cerr.rdbuf(&sbuf);
        std::cerr << std::fixed << std::setprecision(precision) << number;
        std::cerr.rdbuf(orig);
        return sbuf.count();
      }
      template <typename NIND>
      size_t println(NIND number, int precision) {
        return print(number, precision) + println();
      }

      template <typename First, typename... Rest>
      size_t print(First&& first, Rest&&... rest) {
        using MYTYPE = typename std::conditional< \
          std::is_same<const __FlashStringHelper*, First>::value, const char*, First >::type;
          MYTYPE _first = reinterpret_cast<MYTYPE>(first);

        countbuf sbuf(std::cerr.rdbuf());
        std::streambuf* orig = std::cerr.rdbuf(&sbuf);
        std::cerr << std::forward<MYTYPE>(_first);
        std::cerr.rdbuf(orig);
        return sbuf.count() + print(std::forward<Rest>(rest)...);
      }

      template <typename First, typename... Rest>
      size_t println(First&& first, Rest&&... rest) {
        using MYTYPE = typename std::conditional< \
          std::is_same<const __FlashStringHelper*, First>::value, const char*, First>::type;
        MYTYPE _first = reinterpret_cast<MYTYPE>(first);

        countbuf sbuf(std::cerr.rdbuf());
        std::streambuf* orig = std::cerr.rdbuf(&sbuf);
        std::cerr << std::forward<MYTYPE>(_first);
        std::cerr.rdbuf(orig);
        return sbuf.count() + println(std::forward<Rest>(rest)...);
      }

      size_t print(const MockPrintable &p) {
        return 0; // TODO
      }
      size_t println(const MockPrintable &p) {
        std::cerr << std::endl; // TODO
        return 1;
      }

      size_t write(uint8_t c) {
        std::cerr << c;
        return 1;
      }
      size_t write(const uint8_t *buffer, size_t size) {
        std::cerr.write(reinterpret_cast<char*>(const_cast<uint8_t*>(buffer)), size);
        return size;
      }
      inline size_t write(const char * buffer, size_t size)
      { return write((uint8_t*) buffer, size); }
      inline size_t write(const char * s)
      { return write((uint8_t*) s, strlen(s)); }
      inline size_t write(unsigned long n)
      { return write((uint8_t) n); }
      inline size_t write(long n)
      { return write((uint8_t) n); }
      inline size_t write(unsigned int n)
      { return write((uint8_t) n); }
      inline size_t write(int n)
      { return write((uint8_t) n); }
    };

    extern _BIN BIN;
    extern MockDebugSerial Serial;
    typedef MockDebugSerial HardwareSerial;
    typedef MockPrint Print;
    typedef MockPrintable Printable;

  #endif

  #ifndef RADIOLIB_MY_SUPER_CUSTOM
    #define RADIOLIB_MY_SUPER_CUSTOM

    #define RADIOLIB_PLATFORM                           "xPI, Linux"


    #define RADIOLIB_PIN_TYPE                           int
    #define RADIOLIB_PIN_MODE                           int
    #define RADIOLIB_PIN_STATUS                         int

    #define RADIOLIB_INTERRUPT_STATUS                   RADIOLIB_PIN_STATUS
    #define RADIOLIB_DIGITAL_PIN_TO_INTERRUPT(p)        digitalPinToInterrupt(p)

    #define RADIOLIB_NC                                 (std::numeric_limits<uint32_t>::min())

    #define RADIOLIB_DEFAULT_SPI                        SPI
    #define RADIOLIB_DEFAULT_SPI_SETTINGS               SPISettings(2000000, MSBFIRST, SPI_MODE0)

    #define RADIOLIB_NONVOLATILE                        PROGMEM
    #define RADIOLIB_NONVOLATILE_READ_BYTE(addr)        pgm_read_byte(addr)
    #define RADIOLIB_NONVOLATILE_READ_DWORD(addr)       pgm_read_dword(addr)
    #define RADIOLIB_TYPE_ALIAS(type, alias)            using alias = type;

    #define RADIOLIB_CB_ARGS_PIN_MODE                   (void, pinMode, int pin, int mode)
    #define RADIOLIB_CB_ARGS_DIGITAL_WRITE              (void, digitalWrite, int pin, int value)
    #define RADIOLIB_CB_ARGS_DIGITAL_READ               (int, digitalRead, int pin)
    #define RADIOLIB_CB_ARGS_TONE                       (void, tone, int _pin, int frequency, unsigned int duration)
    #define RADIOLIB_CB_ARGS_NO_TONE                    (void, noTone, int _pin)
    #define RADIOLIB_CB_ARGS_ATTACH_INTERRUPT           (void, attachInterrupt, uint8_t interruptNum, void (*userFunc)(void), int mode)
    #define RADIOLIB_CB_ARGS_DETACH_INTERRUPT           (void, detachInterrupt, uint8_t interruptNum)
    #define RADIOLIB_CB_ARGS_YIELD                      (void, yield, void)
    #define RADIOLIB_CB_ARGS_DELAY                      (void, delay, unsigned int ms)
    #define RADIOLIB_CB_ARGS_DELAY_MICROSECONDS         (void, delayMicroseconds, unsigned int us)
    #define RADIOLIB_CB_ARGS_MILLIS                     (unsigned int, millis, void)
    #define RADIOLIB_CB_ARGS_MICROS                     (unsigned int, micros, void)
    #define RADIOLIB_CB_ARGS_PULSE_IN                   (unsigned long, pulseIn, uint8_t pin, uint8_t state, unsigned long timeout)
    #define RADIOLIB_CB_ARGS_SPI_BEGIN                  (void, SPIbegin, void)
    #define RADIOLIB_CB_ARGS_SPI_BEGIN_TRANSACTION      (void, SPIbeginTransaction, void)
    #define RADIOLIB_CB_ARGS_SPI_TRANSFER               (uint8_t, SPItransfer, uint8_t b)
    #define RADIOLIB_CB_ARGS_SPI_END_TRANSACTION        (void, SPIendTransaction, void)
    #define RADIOLIB_CB_ARGS_SPI_END                    (void, SPIend, void)


    #define RADIOLIB_EEPROM_UNSUPPORTED                 (1)
    #define RADIOLIB_TONE_UNSUPPORTED                   (1)
    #define RADIOLIB_EXCLUDE_CC1101                     (1)
    #define RADIOLIB_EXCLUDE_NRF24                      (1)
    #define RADIOLIB_EXCLUDE_SI443X                     (1)
    #define RADIOLIB_EXCLUDE_RFM2X                      (1)
    #define RADIOLIB_EXCLUDE_STM32WLX                   (1)
    #define RADIOLIB_EXCLUDE_AFSK                       (1)
    #define RADIOLIB_EXCLUDE_APRS                       (1)
    #define RADIOLIB_EXCLUDE_AX25                       (1)
    #define RADIOLIB_EXCLUDE_HELLSCHREIBER              (1)
    #define RADIOLIB_EXCLUDE_MORSE                      (1)
    #define RADIOLIB_EXCLUDE_RTTY                       (1)
    #define RADIOLIB_EXCLUDE_SSTV                       (1)
    #define RADIOLIB_EXCLUDE_BELL                       (1)
  #endif

#endif
