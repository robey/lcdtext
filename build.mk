
# set these to your chip and arduino home.

DEVICE         := atmega328p
CLOCK          := 16000000L
PORT           := /dev/cu.usbmodem*

ARDUINO_HOME   := /Applications/Arduino.app/Contents/Resources/Java

# everything below here should be the same everywhere.

ARDUINO_TOOLS  := $(ARDUINO_HOME)/hardware/tools
ARDUINO_CORES  := $(ARDUINO_HOME)/hardware/arduino/cores/arduino
ARDUINO_BIN    := $(ARDUINO_TOOLS)/avr/bin
ARDUINO_ETC    := $(ARDUINO_TOOLS)/avr/etc
CC             := $(ARDUINO_BIN)/avr-gcc
CXX            := $(ARDUINO_BIN)/avr-g++
AR             := $(ARDUINO_BIN)/avr-ar
OBJCOPY        := $(ARDUINO_BIN)/avr-objcopy
AVRDUDE        := $(ARDUINO_BIN)/avrdude

CC_DEBUG       := -g
CC_OPTIMIZE    := -Os
CC_TWEAKS      := \
  -ffunction-sections \
  -fdata-sections \
  -fpack-struct \
  -fshort-enums
CC_PLATFORM    := -mmcu=$(DEVICE) -DF_CPU=$(CLOCK)
CC_INCLUDES    := \
  -I$(ARDUINO_TOOLS)/avr/avr-4/include \
  -I$(ARDUINO_CORES) \
  -I$(ARDUINO_HOME)/hardware/arduino/variants/standard

CC_OPTIONS     := $(CC_DEBUG) $(CC_OPTIMIZE) $(CC_TWEAKS) $(CC_PLATFORM) $(CC_INCLUDES) -Wall -Werror -Wno-sign-compare

CXX_TWEAKS     := \
  -fno-exceptions

CXX_OPTIONS    := $(CC_OPTIONS) $(CXX_TWEAKS)

OBJ = $(addprefix $(TARGET)/, $(SOURCE:.cpp=.o))
TEST_OBJ = $(addprefix $(TARGET)/, $(TEST_SOURCE:.cpp=.o))

AVRDUDE_PROGRAMMER := arduino
AVRDUDE_WRITE_FLASH := -U flash:w:$(TARGET)/$(BIN).hex
AVRDUDE_FLAGS := -C $(ARDUINO_ETC)/avrdude.conf -F -p $(DEVICE) -P $(PORT) -c $(AVRDUDE_PROGRAMMER) -b 115200 -D

# basic rules.

$(TARGET):
	mkdir -p $(TARGET)

clean:
	rm -rf $(TARGET)

vpath %.c $(ARDUINO_CORES)
vpath %.cpp $(ARDUINO_CORES)

.cpp.s:
	$(CXX) -S $(CXX_OPTIONS) $< -o $(TARGET)/$@

$(TARGET)/%.o: %.cpp
	$(CXX) -c -MMD $(CXX_OPTIONS) $< -o $@
$(TARGET)/%.o: %.c
	$(CC) -c -MMD $(CC_OPTIONS) $< -o $@

.PHONY: clean

# making the arduino library.

LIBARDUINO_C_SOURCE := $(addprefix $(ARDUINO_CORES)/, \
  WInterrupts.c wiring.c wiring_analog.c wiring_digital.c wiring_pulse.c \
  wiring_shift.c)
LIBARDUINO_CXX_SOURCE := \
  CDC.cpp HardwareSerial.cpp HID.cpp IPAddress.cpp new.cpp  \
  Stream.cpp USBCore.cpp WMath.cpp 
# things i don't care about: Print.cpp Tone.cpp WString.cpp

LIBARDUINO_LIB := $(TARGET)/libarduino.a
LIBARDUINO_OBJ := $(addprefix $(TARGET)/, $(notdir $(LIBARDUINO_C_SOURCE:.c=.o) $(LIBARDUINO_CXX_SOURCE:.cpp=.o)))

$(LIBARDUINO_LIB): $(LIBARDUINO_OBJ)
	$(AR) rcs $@ $+

# program the device.

upload: $(TARGET)/$(BIN).hex #$(TARGET)/$(BIN).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

wut:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:sketch_lcd.cpp.hex:i

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

%.eep: %.elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $< $@


#avr-gcc -Os -Wl,--gc-sections -mmcu=atmega328p
#  -o /var/folders/td/f0_v0yys6kjbr9vx3n5yhs5m0000gn/T/build3672965930297235545.tmp/sketch_lcd.cpp.elf
#  /var/folders/td/f0_v0yys6kjbr9vx3n5yhs5m0000gn/T/build3672965930297235545.tmp/sketch_lcd.cpp.o
#  /var/folders/td/f0_v0yys6kjbr9vx3n5yhs5m0000gn/T/build3672965930297235545.tmp/core.a
#  -L/var/folders/td/f0_v0yys6kjbr9vx3n5yhs5m0000gn/T/build3672965930297235545.tmp -lm 
#avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 /var/folders/td/f0_v0yys6kjbr9vx3n5yhs5m0000gn/T/build3672965930297235545.tmp/sketch_lcd.cpp.elf /var/folders/td/f0_v0yys6kjbr9vx3n5yhs5m0000gn/T/build3672965930297235545.tmp/sketch_lcd.cpp.eep 
#avr-objcopy -O ihex -R .eeprom 
#  /var/folders/td/f0_v0yys6kjbr9vx3n5yhs5m0000gn/T/build3672965930297235545.tmp/sketch_lcd.cpp.elf 
#  /var/folders/td/f0_v0yys6kjbr9vx3n5yhs5m0000gn/T/build3672965930297235545.tmp/sketch_lcd.cpp.hex 
#
#avr-gcc -Os -mmcu=atmega328p -DF_CPU=16000000L -Wl,--gc-sections 
#  target/test.o target/liblcdtext.a target/libarduino.a -o target/test.elf

#build/%.o: $(ARDUINO_CORES)/%.cpp
#	$(CXX) -c -MMD $(CXX_OPTIONS) $< -o $@ -Wno-error
#build/%.o: $(ARDUINO_CORES)/%.c
#	$(CC) -c -MMD $(CC_OPTIONS) $< -o $@
