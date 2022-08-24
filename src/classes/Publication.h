#include <Arduino.h>

#ifndef PUBLICATION_H
#define PUBLICATION_H



class Publication {       // The class
    private:             // Access specifier
        String topic;
        String publication;

    public: 
        Publication();
        Publication(String, String);

        String getTopic();
        String getPublication();       
};

#endif