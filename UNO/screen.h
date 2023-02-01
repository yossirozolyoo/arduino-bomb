#pragma once

#include "StatusLED.h" 

namespace StatusLEDs {
    enum StatusLEDs {
        RED,
        GREEN,
        YELLOW,
        BLUE,
        WHITE,

        NUM_LEDS
    };
};

extern StatusLED status_leds[];

void setup_screen();
void refresh_screen();
void delay_and_refresh_screen(unsigned long ms);