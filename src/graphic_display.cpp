#include "graphic_display.h"

#include <ranges>
#include <string>
#include <U8g2lib.h>

extern "C" {
/*
  Fontname: Maze
  Copyright: Created with Fony 1.4.7
  Glyphs: 16/16
  BBX Build Mode: 0
*/
const uint8_t maze_font[153] U8G2_FONT_SECTION("maze_font") =
        "\20\0\4\4\4\4\4\4\5\10\10\0\0\0\0\10\0\0\0\0\0\0\200 \5\0\210\30!\6\30\370"
        "\30\20\42\6\201\217\30\20#\10\210\210\30\320\305\17$\6\30\210\30\20%\11\210\210\30\320\303\6\1&"
        "\10\210\210\370\342\273\4'\11\210\210\30\320\305\273\4(\6\201\210\30\20)\11\210\210\30\322\305w\0*"
        "\11\210\210\30\202\311\317\2+\11\210\210\30\222\311g\1,\11\210\210\30\302\305w\10-\11\210\210\30\322"
        "\305;\4.\11\210\210\30\202\311g\11/\11\210\210\30\222\311\263\4\0\0\0";
}

#ifdef WOKWI
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
#else
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0,
                                           /* cs= */ graphic_display::PIN_CS,
                                           /* dc= */ graphic_display::PIN_DC,
                                           /* reset= */ graphic_display::PIN_RST);
#endif

static void drawStrMultiline(auto xOff, auto yOff, const char *s);


void graphic_display::setup() {
    log_d("Graphic display setup");
    u8g2.begin();
    u8g2.setFont(u8g2_font_ncenB08_tf);
}

void graphic_display::clear() {
    u8g2.clear();
}

void graphic_display::setCursor(uint8_t x, uint8_t y) { u8g2.setCursor(x, y); }

void graphic_display::draw(const char *s, Type type) {
    u8g2.clearBuffer();
    uint8_t drawColor = 1;
    uint8_t fontMode = 1;
    uint8_t xOff = 4;
    uint8_t yOff = 4;
    using enum Type;
    switch (type) {
        case NORMAL:
            log_d("Drawing normal text: \n%s", s);
            u8g2.setFont(u8g2_font_ncenB08_tf);
            break;
        case CODE: // TODO: set font and offset
            log_d("Drawing code text: \n%s", s);
            u8g2.setFont(u8g2_font_ncenB08_tf);
            break;
        case MAZE:
            log_d("Drawing maze text: \n%s", s);
            u8g2.setFont(maze_font);
            drawColor = 0;
            fontMode = 0;
            xOff = 32;
            yOff = 0;
            break;
    }
    // u8g2.setDrawColor(drawColor);
    // u8g2.setFontMode(fontMode);
    drawStrMultiline(xOff, yOff, s);
    u8g2.sendBuffer();
}


static void drawStrMultiline(auto xOff, auto yOff, const char *s) {
    auto height = u8g2.getMaxCharHeight();
    using namespace std;
    auto lines = string_view(s) |
                 views::split("\n"sv) |
                 views::transform([](auto r) { return string{data(r), size(r)}; });
    for (uint8_t i = 1; const string &line : lines) {
        u8g2.drawStr(xOff, yOff + height * i, line.c_str());
        ++i;
    }
}
