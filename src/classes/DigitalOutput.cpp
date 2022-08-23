#include <Arduino.h>
#include "Port.h"
#include "DigitalOutput.h"
#include <cmath>

DigitalOutput :: DigitalOutput() : Port () {};

// Ventilador(int pin, bool nl) : Atuador(pin, nl)

DigitalOutput :: DigitalOutput(int pino, int tipo, int periodo, bool nivelLogico) : Port(pino, periodo) {
    this -> tipo = tipo;
    this -> nivelLogico = nivelLogico;
    this -> state = false;
};

void DigitalOutput :: begin(){
    pinMode(pino, OUTPUT);  
}

void DigitalOutput :: turnOn(){
    digitalWrite(pino, nivelLogico);
    this -> state = true;
}

void DigitalOutput :: turnOff(){
    digitalWrite(pino, !nivelLogico);
    this -> state = false;
}

void DigitalOutput :: toggle(){
    digitalWrite(pino, !digitalRead(pino));
}
