#pragma once
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "lgfx_ESP32_2432S028.h"

enum PalColor {Black, White, Gray1, Gray2, Red, Gray3, Gray4 };

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
        uint32_t pal[16] = {
            0x000000, 0xFFFFFF, 0xC0C0C0, 0x808080,
            0xFF0000, 0x202020, 0xA0A0A0,
            0,0,0,0,0,0,0,0
        };

        void applyPalette(LGFX_Sprite& s);
        void createTicks();
        void createHands();
};
