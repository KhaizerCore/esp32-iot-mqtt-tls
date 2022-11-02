#ifndef PORT_H
#define PORT_H

class Port {       // The class
    public:             // Access specifier
        int pino;        
        uint8_t mode;

    Port();
    Port(int, uint8_t);

    public: 
        void begin();      
};

#endif