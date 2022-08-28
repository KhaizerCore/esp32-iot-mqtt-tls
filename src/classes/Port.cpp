#include <Arduino.h>
#include "Port.h"
#include <cmath>

Port::Port(){}

Port::Port(int pino, int periodo, uint8_t mode) {
    this -> pino = pino;
    this -> periodoAmostragem = periodo;
    this -> mode = mode;
};

void Port :: begin(){
    pinMode(pino, mode);  
}