CC = g++
LIBS = -lm -lpthread -lrt -lcrypt
CFLAGS = -std=c++14 -Wall -DLINUX -DARDUINO=999 -DNOWIRINGIPI -Isrc/ -Isrc/modules/ -Isrc/linux-workarounds/ -Isrc/linux-workarounds/SPI/

# Should be equivalent to your list of C files, if you don't build selectively
SRC = $(wildcard src/linux-workarounds/SPI/*.cpp) $(wildcard src/linux-workarounds/*.cpp) \
      $(wildcard src/protocols/PhysicalLayer/*.cpp) $(wildcard src/modules/RF69/*.cpp) \
      $(wildcard src/modules/RFM9x/*.cpp) $(wildcard src/modules/SX127x/*.cpp) \
      $(wildcard src/modules/SX128x/*.cpp) $(wildcard src/modules/SX126x/*.cpp) \
      $(wildcard src/modules/SX1231/*.cpp) $(wildcard src/*.cpp) $(wildcard *.cpp)

all: $(SRC)
	$(CC) -o test $^ $(CFLAGS) -O3 $(LIBS)

debug: $(SRC)
	$(CC) -o test $^ -g3 -DRADIOLIB_DEBUG $(CFLAGS) $(LIBS)

clean:
	rm -f ./test
