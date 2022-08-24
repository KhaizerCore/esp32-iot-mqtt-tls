#include "Publication.h"
#include <Arduino.h>
#include <vector>
using namespace std;

#ifndef PUBLICATIONQUEUE_H
#define PUBLICATIONQUEUE_H



class PublicationQueue {       // The class
    private:             // Access specifier
        vector<Publication> publicationQueue;

    public: 
        PublicationQueue();

        Publication front();         
        Publication pop_front();
        void push(Publication); 
        bool empty();
};

#endif