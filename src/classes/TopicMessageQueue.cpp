#include "TopicMessageQueue.h"

TopicMessageQueue :: TopicMessageQueue() {
    this -> topicMessageQueue = vector<TopicMessage>();
};


TopicMessage TopicMessageQueue :: front(){
    return topicMessageQueue.front();
}

TopicMessage TopicMessageQueue :: pop_front(){
    TopicMessage ret = topicMessageQueue.front();
    topicMessageQueue.erase(topicMessageQueue.begin());
    return ret;
}

void TopicMessageQueue :: push(TopicMessage publication) {
    topicMessageQueue.push_back(publication);
}

bool TopicMessageQueue :: empty() {
    return topicMessageQueue.empty();
}