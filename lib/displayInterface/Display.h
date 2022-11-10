#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>
#include <displayFonts.h>

class Display {       // The class
    public:             // Access specifier
        U8G2_SSD1309_128X64_NONAME0_1_4W_SW_SPI lcd;

        Display();

        void begin();
        void init();
        void printTest();
        void clear();
        void print(String, int, int);
};

#endif