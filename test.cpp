#include "lcdtext.h"

LcdText text;

int main() {
  init();

  text.init();
  text.setColor(0xff0);

  text.setBackgroundColor(0x00f);
  text.clear();
  text.setBackgroundColor(0x0f0);
  text.clear();
  text.setBackgroundColor(0xf00);
  text.clear();

  text.drawChar(0, 0, 0);
  text.drawChar(1, 1, 0);
  text.setCursor(0, 3);
  text.drawText("test!");
  
  text.setCursor(2, 0);
  text.drawText("This is a test of drawing strings to the LCD screen. \x1f");
  
  text.setCursor(6, 0);
  text.setColor(0x0f0);
  text.setBackgroundColor(0x000);
  text.drawText("As an example of how quickly it can draw, here is a much longer piece of text, in a different color.");

  for (;;);
}
