#include <Arduino.h>
#include "Port.h"
#include "DigitalOutput.h"

DigitalOutput :: DigitalOutput() : Port () {};

// Ventilador(int pin, bool nl) : Atuador(pin, nl)

DigitalOutput :: DigitalOutput(int pino, int tipo, int periodo, bool nivelLogico) : Port(pino, periodo, OUTPUT) {
    this -> tipo = tipo;
    this -> nivelLogico = nivelLogico;
};

void DigitalOutput :: turnOn(){
    digitalWrite(pino, nivelLogico);
}

void DigitalOutput :: turnOff(){
    digitalWrite(pino, !nivelLogico);
}

void DigitalOutput :: toggle(){
    digitalWrite(pino, !digitalRead(pino));
}

bool DigitalOutput :: getPinValue(){
    return digitalRead(this -> pino);
}

/*
NL INPUT state
 0   0     1
 0   1     0
 1   0     0
 1   1     1
*/
bool DigitalOutput :: getState(){
    return digitalRead(this -> pino) == this -> nivelLogico;
}
