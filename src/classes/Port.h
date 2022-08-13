#ifndef PORT_H
#define PORT_H

class Port {       // The class
    public:             // Access specifier
        int pino;        
        int periodoAmostragem;

    Port();
    Port(int, int);

    public: 
        void begin();      
};

#endif