#include "TopicMessage.h"
#include <Arduino.h>
#include <vector>
using namespace std;

#ifndef TOPICMESSAGEQUEUE_H
#define TOPICMESSAGEQUEUE_H



class TopicMessageQueue {       // The class
    private:             // Access specifier
        vector<TopicMessage> topicMessageQueue;

    public: 
        TopicMessageQueue();

        TopicMessage front();         
        TopicMessage pop_front();
        void push(TopicMessage); 
        bool empty();
};

#endif