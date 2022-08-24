#include "Publication.h"

Publication :: Publication(String topic, String publication) {
    this -> topic = topic;
    this -> publication = publication;
};

String Publication :: getTopic() {
    return topic;
}

String Publication :: getPublication(){
    return publication;
}
