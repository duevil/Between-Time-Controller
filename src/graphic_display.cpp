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
#endif/*
    /*
    Fontname: Mayan Symbols
    Copyright: Created with Fony 1.4.7
    Glyphs: 10/10
    BBX Build Mode: 0
    */
const uint8_t mayan_symbols[618] U8G2_FONT_SECTION("mayan_symbols") =
        "\12\0\3\2\5\5\2\2\6\23\23\1\1\24\0\24\0\0\0\0\0\2Q\60\71s~\255\341\333\16."
        "\305AL\224\314\226\270*\246!\35\42q\70\352\300\16\350\300\16\350\300\16\210\312\42Z\26Q\261,\242"
        "bY|Y|Y|Y,\311\242,\312\66|\2\61\63s~\255\341[N\251\323DU\264\212\232\16"
        "h\312\240\16\212e[\254\303\220\252\303\220Z\22\325\222\250\226D\265$\252MU\207!\225\7\71\311)"
        "\331\360\11\62@s~\255\341[\16\344@\222#\71\62\214\341\260i\231\66(R\244\14\332\24mjT"
        "\325\201\35\320\201$\7dE\26\267Q\31r`H\6\35\34t`\7De\21\265\341\240%\231\262h"
        "\331\360\11\63Hs~\255\341[\16H\221\222#K\224h\223\222(\223\22)\65E\212\224\232\22Y\22"
        "eI\264%J$q\211\22\35\32\16\311\260\15O;\244\3\331p\210\224h\370\222,\25e\210\22)"
        "Ide\252d\233\224d\303'\0\64\65s~\255\341\233\62$\226%\31\26/\213O\221\342\223\342\223"
        "\342I\361mH<.\336i\312\240\203C\246ck\216-R\216-R\216\215:\226\14;\226\15\237\0"
        "\65\67s~\255\341[N\251\15\7M\32\36\42E\24\27\257\212QU\244aV\264!\7\24\235\240\350"
        "\4E'(:A\261\203\212\35[\304\341 i\303AKrJ\66|\2\66=s~\255\341[\16\344"
        "@\22\211\71\242\14\321\16(C\264\3\222EV\65U\334Fi\7\246a\207\206i\7&q\33UM"
        "\225,\262\62D;\240\14\321\16Hb\216\324\201\34\310\206O\0\67Fs~\255\341[NI\206\7q"
        "\370(%C\222\3R\62$\71\240\15;\242\323t\332\360\20%\311\360\20%\211:\204I\242\15q\222"
        "H[\22%\211\62&Q\222\14R%J\222!\213\222(i\214\222(\321\206O\0\70\70s~\255\341"
        "[\16.u\60Kt\254\62\15Yi\31\246\332\42II\226\230\264\222\62\214\233\64\344\240N\33\336\201"
        "\17\252\16\15\241\254\35\16\241\66\134\265$\7\245l\370\4\71\64s~\255\341[\216\250I\232\311\342$"
        "\253\222,%\223\254\14\232,m\263\16\14\261<\310\26Y\36dy\310\1yGd\35\222uH\326\241"
        "$\325\221l\370\4\0\0\0";
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
        case CODE:
            log_d("Drawing code text: \n%s", s);
            u8g2.setFont(mayan_symbols);
            xOff = 1;
            yOff = 12;
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
