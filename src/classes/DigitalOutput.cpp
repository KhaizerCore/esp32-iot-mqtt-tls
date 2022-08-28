#include <Arduino.h>
#include "Port.h"
#include "DigitalOutput.h"
#include <cmath>

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
