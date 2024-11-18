#include "graphic_display.h"

#include <ranges>
#include <string>
#include <U8g2lib.h>

extern "C" {
#ifdef SHOW_MAZE
/*
    Fontname: Maze
    Copyright: Created with Fony 1.4.7
    Glyphs: 32/32
    BBX Build Mode: 0
*/
const uint8_t maze_font[340] U8G2_FONT_SECTION("maze_font") =
        " \0\4\4\4\4\4\4\5\10\10\0\0\10\0\10\0\0\0\0\0\1; \5\0\210\30!\6\30\370"
        "\30\20\42\6\201\217\30\20#\10\210\210\30\320\305\17$\6\30\210\30\20%\11\210\210\30\320\303\6\1&"
        "\10\210\210\370\342\273\4'\11\210\210\30\320\305\273\4(\6\201\210\30\20)\11\210\210\30\322\305w\0*"
        "\11\210\210\30\202\311\317\2+\11\210\210\30\222\311g\1,\11\210\210\30\302\305w\10-\11\210\210\30\322"
        "\305;\4.\11\210\210\30\202\311g\11/\11\210\210\30\222\311\263\4\60\6\42\273\30\10\61\13X\270\30"
        "\320\3\22&\6\0\62\13\205\213\230\42\32\62$P\14\63\15\210\210\30\320E#$\214\220p\61\64\12"
        "X\210x\204\211\7\204\0\65\14\210\210\30\320\3\22&\36\20\2\66\15\210\210\370b#$\214\220p\341"
        "\22\67\16\210\210\30\320E#$\214\220p\341\22\70\13\205\210\30\2\305d\242\210\0\71\16\210\210\30\322"
        "E\42&\210\230p\321\1:\13\210\210\30\202\311D\316$\13;\13\210\210\30\222I\42g\222\5<\16"
        "\210\210\30\302\305DL\20\61\341\302!=\16\210\210\30\322E\42&\210\230p\341\20>\13\210\210\30\202"
        "\311D\316\204%?\13\210\210\30\222I\42g\302\22\0\0\0";
#else
/*
    Fontname: Maze Crumbs
    Copyright: Created with Fony 1.4.7
    Glyphs: 3/3
    BBX Build Mode: 0
*/
const uint8_t maze_crumbs[49] U8G2_FONT_SECTION("maze_crumbs") =
        "\3\0\2\3\3\3\3\3\5\7\7\0\0\0\0\0\0\0\0\0\0\0\30 \5\0\211\1!\10\244\215"
        "\223\216H\2\42\11v\213\243\216\307D\1\0\0\0";
#endif
}

#ifdef WOKWI
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
#else
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0,
                                           /* cs= */ graphic_display::PIN_CS,
                                           /* dc= */ graphic_display::PIN_DC,
                                           /* reset= */ graphic_display::PIN_RST);
#endif


void graphic_display::setup() {
    log_d("Graphic display setup");
    u8g2.begin();
    u8g2.setFont(u8g2_font_ncenB08_tf);
}

void graphic_display::clear() { u8g2.clearBuffer(); }

void graphic_display::draw(const char *s, Type type) {
    uint8_t xOff = 4;
    uint8_t yOff = 4;
    using enum Type;
    switch (type) {
        case NORMAL:
            log_d("Drawing normal text: \n%s", s);
            u8g2.setFont(u8g2_font_crox1hb_tr);
            break;
        case CODE: // TODO: set font and offset
            log_d("Drawing code text: \n%s", s);
            u8g2.setFont(u8g2_font_crox1hb_tr);
            break;
        case MAZE:
            log_d("Drawing maze text: \n%s", s);
#ifdef SHOW_MAZE
            u8g2.setFont(maze_font);
#else
            u8g2.setFont(maze_crumbs);
#endif
            xOff = 16;
            yOff = 0;
            break;
        case TIMER:
            log_d("Drawing timer text: \n%s", s);
            u8g2.setFont(u8g2_font_crox1cb_mn);
            xOff = static_cast<uint8_t>(u8g2.getDisplayWidth() - u8g2.getStrWidth(s));
            yOff = static_cast<uint8_t>(u8g2.getDisplayHeight() - u8g2.getMaxCharHeight() - 4);
            break;
    }
    // draw multiline text
    auto height = u8g2.getMaxCharHeight();
    using namespace std;
    auto lines = string_view(s) |
                 views::split("\n"sv) |
                 views::transform([](auto r) { return string{data(r), size(r)}; });
    for (uint8_t i = 1; const string &line : lines) {
        u8g2.drawStr(xOff, yOff + i + height * i, line.c_str());
        ++i;
    }
    u8g2.sendBuffer();
}
