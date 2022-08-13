#include <Arduino.h>
#include "Port.h"
#include <cmath>

Port::Port(){}

Port::Port(int pino, int periodo) {
    this -> pino = pino;
    this -> periodoAmostragem = periodo;
};

void Port :: begin(){
    pinMode(pino, OUTPUT);  
}