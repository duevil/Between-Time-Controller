#include "graphic_display.h"
#include <U8g2lib.h>

#ifdef WOKWI
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
#else
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0,
                                           /* cs= */ graphic_display::PIN_CS,
                                           /* dc= */ graphic_display::PIN_DC,
                                           /* reset= */ graphic_display::PIN_RST);
#endif


template<typename Func>
void draw(const Func &drawFunc) {
    u8g2.clearBuffer();
    drawFunc();
    u8g2.sendBuffer();
}


void graphic_display::setup() {
    log_d("Graphic display setup");
    u8g2.begin();
    u8g2.setFont(u8g2_font_ncenB08_tf);
}

void graphic_display::clear() {
    u8g2.clear();
    u8g2.setFont(u8g2_font_ncenB08_tf);
}

void graphic_display::setCursor(uint8_t x, uint8_t y) { u8g2.setCursor(x, y); }

void graphic_display::drawStr(uint8_t x, uint8_t y, const char *s) {
    draw([&x, &y, &s] { u8g2.drawStr(x, y, s); });
}

void graphic_display::drawInputField() {
    log_d("drawInputField");
    // TODO
}

void graphic_display::drawCode(const char *code) {
    log_d("drawCode: %s", code);
    // TODO
}

void graphic_display::drawMaze(const maze::Position &position) {
    log_d("drawMaze: [%d, %d]", position.x, position.y);
    // TODO
}
