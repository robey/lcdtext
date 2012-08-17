
TARGET := target
SOURCE := lcdtext.cpp
TEST_SOURCE := test.cpp
LIBRARY := $(TARGET)/liblcdtext.a
BIN := test

all: $(TARGET) $(LIBRARY)

include build.mk

$(LIBRARY): $(OBJ)
	$(AR) rcs $(LIBRARY) $(OBJ)

test: $(TARGET) upload

$(TARGET)/test.elf: $(TEST_OBJ) $(LIBRARY) $(LIBARDUINO_LIB)
	$(CC) $(CC_OPTIMIZE) -Wl,--gc-sections $(CC_PLATFORM) -o $@ $+

.PHONY: test all

-include $(TARGET)/*.d
