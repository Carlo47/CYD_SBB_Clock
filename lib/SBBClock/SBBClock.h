#pragma once
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "lgfx_ESP32_2432S028.h"

// ---------- Colors of the 4-bit-palette ----------
enum PalColor {Transparent, Black, White, Red, Green, Blue, Lightgray, Darkgray,
               Yellow, Magenta, Orange, Pink, Brown, Gold, Silver, Skyblue };

class SBBClock 
{
    public:
       SBBClock(LGFX* display) : _lcd(display),
            _canvas(display),    _dial(&_canvas),
            _tickMinute(&_dial), _tickHour(&_dial),
            _minuteHand(&_dial), _hourHand(&_dial),
            _secondHand(&_dial), _paddle(&_dial)
    {}
        void init(int side);
        void update();

    private:
        LGFX* _lcd;
        int   _size, _radius;
        int   _cx, _cy;
        tm       _rtcTime;
        timeval  _tv;

        // Sprites
        LGFX_Sprite _canvas;
        LGFX_Sprite _dial;
        LGFX_Sprite _tickMinute;
        LGFX_Sprite _tickHour;
        LGFX_Sprite _minuteHand;
        LGFX_Sprite _hourHand;
        LGFX_Sprite _secondHand;
        LGFX_Sprite _paddle;

        // Palette
        int pal[16] = {
            TFT_TRANSPARENT, 
            TFT_BLACK,
            TFT_WHITE,
            TFT_RED, 
            TFT_GREEN, 
            TFT_BLUE, 
            TFT_LIGHTGRAY, 
            TFT_DARKGRAY,
            TFT_YELLOW,
            TFT_MAGENTA,
            TFT_ORANGE,
            TFT_PINK,
            TFT_BROWN,
            TFT_GOLD,
            TFT_SILVER,
            TFT_SKYBLUE
        };

        void applyPalette(LGFX_Sprite& s);
        void createTicks();
        void createHands();
};
