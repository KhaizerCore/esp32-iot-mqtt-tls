#include "TopicMessage.h"

TopicMessage :: TopicMessage(String topic, String message) {
    this -> topic = topic;
    this -> message = message;
};

String TopicMessage :: getTopic() {
    return this -> topic;
}

String TopicMessage :: getMessage(){
    return this -> message;
}
