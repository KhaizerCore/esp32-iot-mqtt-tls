#include <Arduino.h>
#include "Port.h"
#include "Lock.h"
#include <cmath>

Lock :: Lock() {
    this -> lock = false;
};

void Lock :: acquire(){
    while (lock){
        vTaskDelay(1 / portTICK_PERIOD_MS);  // wait for a ms  
    }
    lock = true;
}

void Lock :: release(){
    lock = false;
}



