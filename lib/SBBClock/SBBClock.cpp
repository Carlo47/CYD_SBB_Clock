#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "lgfx_ESP32_2432S028.h"

#include "SBBClock.h"


// ---------- Apply the color palette ----------
void SBBClock::applyPalette(LGFX_Sprite& s) {
    for (int i = 0; i < 16; i++)
        s.setPaletteColor(i, pal[i]);
}

// ---------- Initialization ----------
void SBBClock::init(int side) {
    _size = side;
    _radius = _size / 2 - 6;
    _cx = _cy = _size / 2;

    // Canvas
    _canvas.setColorDepth(lgfx::palette_4bit);
    _canvas.createSprite(_size, _size);
    applyPalette(_canvas);
    _canvas.fillScreen(PalColor::Black);

    // Dial
    _dial.setColorDepth(lgfx::palette_4bit);
    _dial.createSprite(_size, _size);
    applyPalette(_dial);
    _dial.fillScreen(PalColor::Lightgray);

    _dial.fillCircle(_cx, _cy, _radius, PalColor::Darkgray);
    _dial.fillCircle(_cx, _cy, _radius * 0.95, PalColor::White);

    createTicks();
    createHands();
}

// ---------- Ticks ----------
void SBBClock::createTicks() {
    float r3 = _radius * 0.87;

    // Minute marks
    {
        int w = _radius * 0.0215;
        int h = _radius * 0.0625;
        _tickMinute.setColorDepth(lgfx::palette_4bit);
        _tickMinute.createSprite(w, h);
        applyPalette(_tickMinute);
        _tickMinute.fillScreen(PalColor::Black);
        _tickMinute.setPivot(w / 2, r3);

        float angle = 0;
        for (int i = 0; i < 60; i++) {
            _tickMinute.pushRotateZoom(&_dial, _cx, _cy, angle, 1.0, 1.0);
            angle += 6;
        }
    }

    // Hour marks
    {
        int w = _radius * 0.059;
        int h = _radius * 0.215;
        _tickHour.setColorDepth(lgfx::palette_4bit);
        _tickHour.createSprite(w, h);
        applyPalette(_tickHour);
        _tickHour.fillScreen(PalColor::Black);
        _tickHour.setPivot(w / 2, r3);

        float angle = 0;
        for (int i = 0; i < 12; i++) {
            _tickHour.pushRotateZoom(&_dial, _cx, _cy, angle, 1.0, 1.0);
            angle += 30;
        }
    }
}

// ---------- Hands ----------
void SBBClock::createHands() {
    // Minute Hand
    {
        int w = _radius * 0.086;
        int h = _radius * 1.043;
        _minuteHand.setColorDepth(lgfx::palette_4bit);
        _minuteHand.createSprite(w, h);
        applyPalette(_minuteHand);
        _minuteHand.fillScreen(PalColor::Black);
        _minuteHand.setPivot(w/2, _radius * 0.839);
    }

    // Hour hand
    {
        int w = _radius * 0.113;
        int h = _radius * 0.780;
        _hourHand.setColorDepth(lgfx::palette_4bit);
        _hourHand.createSprite(w, h);
        applyPalette(_hourHand);
        _hourHand.fillScreen(PalColor::Black);
        _hourHand.setPivot(w / 2, _radius * 0.575);
    }

    // Second hand
    {
        int w = _radius * 0.032;
        int h = _radius * 0.957;
        _secondHand.setColorDepth(lgfx::palette_4bit);
        _secondHand.createSprite(w, h);
        applyPalette(_secondHand);
        _secondHand.fillScreen(PalColor::Red);
        _secondHand.setPivot(w / 2, _radius * 0.645);
    }

    // Paddle
    {
        int w = _radius * 0.183;
        int h = w;
        int r = (w - 1) / 2;
        _paddle.setColorDepth(lgfx::palette_4bit);
        _paddle.createSprite(w, h);
        applyPalette(_paddle);
        _paddle.fillScreen(PalColor::Transparent); 
        _paddle.fillCircle(r, r, r, PalColor::Red); 
        _paddle.setPivot(w / 2, _radius * 0.645);
    }
}

// ---------- Update / Animation ----------
void SBBClock::update() {
    getLocalTime(&_rtcTime);  // Local zone time
    gettimeofday(&_tv, NULL); // Unix time synchronized to local time as seconds sinc 1.1.1970
    uint32_t now = 1000 * (_rtcTime.tm_hour * 3600 + _rtcTime.tm_min * 60 + _rtcTime.tm_sec) + _tv.tv_usec / 1000; 

    // --- Time constant ---
    const uint32_t MS_PER_SEC  = 1000;
    const uint32_t MS_PER_MIN  = 60 * MS_PER_SEC;
    const uint32_t MS_PER_HOUR = 60 * MS_PER_MIN;

    // --- Hand rotation times ---
    const uint32_t MS_PER_SEC_REV  = 60 * MS_PER_SEC;        // 60 seconds
    const uint32_t MS_PER_MIN_REV  = 60 * MS_PER_MIN;        // 60 minutes
    const uint32_t MS_PER_HOUR_REV = 12 * MS_PER_HOUR;       // 12 hours

    // --- Angle per Millisecond ---
    const float DEG_PER_MS_SEC  = 360.0f / MS_PER_SEC_REV;
    const float DEG_PER_MS_MIN  = 360.0f / MS_PER_MIN_REV;
    const float DEG_PER_MS_HOUR = 360.0f / MS_PER_HOUR_REV;

    // --- Angle of hands (always 0–360°) ---
    //float secAngle  = fmod(now * DEG_PER_MS_SEC,  360.0f); // smooth-gliding second hand 
    //float minAngle  = fmod(now * DEG_PER_MS_MIN,  360.0f); // smooth-gliding minute hand
    float hourAngle = fmod(now * DEG_PER_MS_HOUR, 360.0f);   // smooth-gliding hour hand

    // SBB Mechanics
    const float SBB_CYCLE = 60.0f;
    const float SBB_RUN   = 58.5f;

    float t = fmod(now / 1000.0f, SBB_CYCLE);

    float secAngle = (t < SBB_RUN) ? (t / SBB_RUN) * 360.0f : 0.0f; // stop and go second hand
    float minAngle  = _rtcTime.tm_min * 6.0f;     // jumping minute hand
    //float hourAngle = _rtcTime.tm_hour * 30.0f; // jumping hour hand

    // Dial
    _dial.pushSprite(&_canvas, 0, 0);

    // Hands
    _hourHand.pushRotateZoom(&_canvas, _cx, _cy, hourAngle, 1.0, 1.0);
    _minuteHand.pushRotateZoom(&_canvas, _cx, _cy, minAngle, 1.0, 1.0);
    _secondHand.pushRotateZoom(&_canvas, _cx, _cy, secAngle, 1.0, 1.0);
    _paddle.pushRotateZoom(&_canvas, _cx, _cy, secAngle, 1.0, 1.0, 0);

    // Display time
    _canvas.pushSprite(_cx/4, 0);
}
