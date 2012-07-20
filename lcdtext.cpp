
/*
 * the LCD shield uses the SPI clock & data-out pins, but uses pin 9 for
 * chip-select (CS) instead of the "slave select" (SS) pin. it also has a
 * bonus pin 8 for "reset".
 */

#define __port_of(n) ((n < 8) ? PORTD : PORTB)
#define __bit_of(n) (1 << (n & 0x7))
#define __set_bit(n) (__port_of(n) |= __bit_of(n))
#define __clear_bit(n) (__port_of(n) &= ~__bit_of(n))

// clock = pin 13
#define SPI_CLOCK_PIN    13
#define SPI_CLOCK_SET    __set_bit(SPI_CLOCK_PIN)
#define SPI_CLOCK_CLEAR  __clear_bit(SPI_CLOCK_PIN)

// spi out = pin 11
#define SPI_OUT_PIN      11
#define SPI_OUT_SET      __set_bit(SPI_OUT_PIN)
#define SPI_OUT_CLEAR    __clear_bit(SPI_OUT_PIN)

// lcd select = pin 9
#define SELECT_PIN       9
#define SELECT_SET       __set_bit(SELECT_PIN)
#define SELECT_CLEAR     __clear_bit(SELECT_PIN)

// spi select = pin 10
// we don't use this pin but it needs to be set to output mode for SPI to work.
#define SPI_SELECT_PIN   10

#define CHAR_WIDTH 4
#define CHAR_HEIGHT 8

// bits are pixels, left to right, then bottom to top in each column.
const uint32_t zrom_font[128] = {
  0x50262d27, 0x2d265000, 0x80408000, 0x00000000,
  0x242e2400, 0x082a0800, 0x00080000, 0x08080808,
  0x00ff0000, 0x00fc0404, 0x04fc0000, 0x080f0000,
  0x000f0808, 0x00ff0808, 0x08f80808, 0x08ff0000,
  0x080f0808, 0x08ff0808, 0x663399cc, 0x993366cc,
  0xfef8e080, 0x7f1f0701, 0x01071f7f, 0x80e0f8fe,
  0x5500aa00, 0x55aa55aa, 0xffaaff55, 0x0f0f0f0f,
  0xf0f0f0f0, 0x0000ffff, 0xffff0000, 0xffffffff,
  0x00000000, 0x005f0000, 0x07000700, 0x3f123f00,
  0x246b1200, 0x12082400, 0x36493650, 0x00040300,
  0x1c224100, 0x41221c00, 0x2a1c2a00, 0x081c0800,
  0x80600000, 0x08080800, 0x00400000, 0x601c0300,
  0x7e413f00, 0x427f4000, 0x72494600, 0x22493600,
  0x0f087f00, 0x27453900, 0x3e493200, 0x71090700,
  0x36493600, 0x26493e00, 0x00140000, 0x40340000,
  0x08142200, 0x14141400, 0x22140800, 0x01590600,
  0x3e594e00, 0x7e097e00, 0x7f493600, 0x3e414100,
  0x7f413e00, 0x7f494900, 0x7f090900, 0x3e413900,
  0x7f087f00, 0x417f4100, 0x60403f00, 0x7f1c6300,
  0x7f404000, 0x7f0e7f00, 0x7f017e00, 0x3e413e00,
  0x7f090600, 0x3e61be00, 0x7f097600, 0x46493100,
  0x017f0100, 0x3f407f00, 0x0f700f00, 0x7f387f00,
  0x77087700, 0x07780700, 0x71494700, 0x7f410000,
  0x031c6000, 0x00417f00, 0x02010200, 0x80808080,
  0x00030400, 0x64547800, 0x7f443800, 0x38444400,
  0x38447f00, 0x38545800, 0x047e0500, 0x98a47c00,
  0x7f047800, 0x007d0000, 0x80807d00, 0x7f106c00,
  0x007f0000, 0x7c187c00, 0x7c047800, 0x38443800,
  0xfc443800, 0x3844fc80, 0x78040400, 0x48542400,
  0x043f4400, 0x3c407c00, 0x1c701c00, 0x7c307c00,
  0x6c106c00, 0x9ca07c00, 0x64544c00, 0x08364100,
  0x00770000, 0x41360800, 0x08040804, 0x02050200,
};

class LcdText {
private:
  static const uint8_t SCREEN_WIDTH = 131;
  static const uint8_t SCREEN_HEIGHT = 132;
  static const uint16_t SCREEN_PIXELS = (uint16_t)SCREEN_WIDTH * (uint16_t)SCREEN_HEIGHT;
  static const uint8_t SCREEN_X_OFFSET = 2;
  static const uint8_t SCREEN_Y_OFFSET = 2;

  static const int RESET = 8;

  // fields for SPI control register: 4MHz, MSB, falling-edge clock trigger.
  static const int SPI_ON = 0x58;
  static const int SPI_OFF = 0x18;

// in theory, there are two different drivers. in reality, i don't care.
#ifdef LCD_DRIVER_EPSON
  static const uint8_t SetPage = 0x75;
  static const uint8_t SetColumn = 0x15;
  static const uint8_t WriteData = 0x5C;
  static const uint8_t Nop = 0x25;
#else /* normal phillips driver */
  static const uint8_t SetPage = 0x2B;
  static const uint8_t SetColumn = 0x2A;
  static const uint8_t WriteData = 0x2C;
  static const uint8_t Nop = 0x00;
#endif

  int cursor;

  // color is, confusingly, in 0BGR format.
  int fgcolor;
  int bgcolor;

  void send(uint8_t is_command, uint8_t data);

public:
  LcdText();
  void sendData(uint8_t data) { send(0, data); }
  void sendCommand(uint8_t data) { send(1, data); }
  void clear();
  void drawChar(uint8_t c, uint8_t x, uint8_t y);
  void drawText(char *s);
  void setColor(int color) { fgcolor = color; }
  void setBackgroundColor(int color) { bgcolor = color; }
  void setCursor(uint8_t y, uint8_t x) { cursor = (y << 5) | x; }
};

LcdText::LcdText() : cursor(0) {
  pinMode(SPI_CLOCK_PIN, OUTPUT);
  pinMode(SPI_OUT_PIN, OUTPUT);
  pinMode(SELECT_PIN, OUTPUT);
  pinMode(RESET, OUTPUT);

  pinMode(SPI_SELECT_PIN, OUTPUT);

  // ensure that the LCD isn't selected, and all lines are in default states.
  SELECT_SET;
  SPI_OUT_CLEAR;
  SPI_CLOCK_SET;
}

void LcdText::send(uint8_t is_command, uint8_t data) {
  SELECT_CLEAR;

  // send the command/data bit out manually.
  if (!is_command) {
    SPI_OUT_SET;
  } else {
    SPI_OUT_CLEAR;
  }
  SPI_CLOCK_CLEAR;
  SPI_CLOCK_SET;

  // now a byte of SPI. this is faster than we can bit-bang.
  SPCR = SPI_ON;
  SPDR = data;
  while (!(SPSR & (1 << SPIF)));
  SPCR = SPI_OFF;

  SELECT_SET;
}

void LcdText::clear() {
  sendCommand(SetPage);
  sendData(0);
  sendData(SCREEN_HEIGHT);
  sendCommand(SetColumn);
  sendData(0);
  sendData(SCREEN_WIDTH);

  sendCommand(WriteData);
  // we have to send 2 pixels at a time, since they're packed into 1.5 bytes
  // per pixel.
  uint8_t byte1 = (bgcolor >> 4) & 0xff;
  uint8_t byte2 = ((bgcolor & 0xf) << 4) | (bgcolor >> 8);
  uint8_t byte3 = bgcolor & 0xff;
  for (uint16_t i = 0; i < (SCREEN_PIXELS >> 1); i++) {
    sendData(byte1);
    sendData(byte2);
    sendData(byte3);
  }

  sendCommand(Nop);
  cursor = 0;
}

void LcdText::drawChar(uint8_t c, uint8_t x, uint8_t y) {
  /*
   * re-orient (x, y): driver considers (0, 0) to be the upper right, with
   * x and y reversed.
   */
  // re-orient (x, y)
  uint8_t px = (y * CHAR_HEIGHT) + SCREEN_X_OFFSET;
  uint8_t py = SCREEN_HEIGHT - (x * CHAR_WIDTH) - SCREEN_Y_OFFSET;

  uint32_t data = zrom_font[c];

  // compute fg/bg pixel pairs for the 4 possible 2-pixel combos.
  uint8_t color_table[16];
  uint8_t index = 0;
  for (uint8_t combo = 0; combo < 4; combo++) {
    int p0 = (combo & 2) ? fgcolor : bgcolor;
    int p1 = (combo & 1) ? fgcolor : bgcolor;
    color_table[index++] = (p1 >> 4) & 0xff;
    color_table[index++] = ((p1 & 0xf) << 4) | (p0 >> 8);
    color_table[index++] = p0 & 0xff;
    index++;
  }

  // bounding box
  sendCommand(SetPage);
  sendData(py - CHAR_WIDTH - 1);
  sendData(py);
  sendCommand(SetColumn);
  sendData(px);
  sendData(px + CHAR_HEIGHT - 1);

  // 2 pixels for each write.
  sendCommand(WriteData);

  for (uint8_t i = 0; i < 16; i++) {
    uint8_t n = (data & 0x3) << 2;
    sendData(color_table[n++]);
    sendData(color_table[n++]);
    sendData(color_table[n++]);
    data >>= 2;
  }

  // seems to be necessary to force the final pixel to latch.
  sendCommand(Nop);
}

void LcdText::drawText(char *s) {
  while (char c = *s++) {
    uint8_t y = cursor >> 5;
    uint8_t x = cursor & 31;
    drawChar(c, x, y);
    cursor++;
  }
}
