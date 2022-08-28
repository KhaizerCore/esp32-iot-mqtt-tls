#include "Port.h"

#ifndef DIGITALOUTPUT_H
#define DIGITALOUTPUT_H

class DigitalOutput : public Port {       // The class
    public:             // Access specifier
        int tipo;    
        bool nivelLogico;

    DigitalOutput();
    DigitalOutput(int, int, int, bool);

    public: 
        void turnOn();
        void turnOff();
        void toggle();        
};

#endif