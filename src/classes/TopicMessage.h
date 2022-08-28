#include <Arduino.h>

#ifndef TOPICMESSAGE_H
#define TOPICMESSAGE_H



class TopicMessage {       // The class
    private:             // Access specifier
        String topic;
        String message;

    public: 
        TopicMessage();
        TopicMessage(String, String);

        String getTopic();
        String getMessage();       
};

#endif