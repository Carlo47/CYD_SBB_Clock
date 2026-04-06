#include "SBBClock.h"

extern void initWiFi();
extern void initRTC(const char *timeZone, bool disconnect = false);
extern void printConnectionDetails();
extern void printNearbyNetworks();
extern void printDateTime(int format);
extern const char *MEZ_MESZ;
extern const char *EST_EDT;
const char *timeZone = MEZ_MESZ; 
const int  TIME_FORMAT = 5;  // 0..6 see function printDateTime()

LGFX lcd;

SBBClock sbbClock(&lcd);

void setup() 
{
    Serial.begin(115200);
    initWiFi();
    printNearbyNetworks();
    printConnectionDetails();
    initRTC(timeZone, false);
    tm rtcTime;
    getLocalTime(&rtcTime);  // Local zone time
    printDateTime(TIME_FORMAT);

    lcd.init();
    lcd.setRotation(3);

    int side = min(lcd.width(), lcd.height());
    sbbClock.init(side);
}

void loop() 
{
    sbbClock.update();
}
