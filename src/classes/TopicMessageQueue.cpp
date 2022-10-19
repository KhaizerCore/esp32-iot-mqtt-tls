#include "TopicMessageQueue.h"

TopicMessageQueue :: TopicMessageQueue() {
    this -> queue = vector<TopicMessage>();
};


TopicMessage TopicMessageQueue :: front(){
    return queue.front();
}

TopicMessage TopicMessageQueue :: pop_front(){
    TopicMessage ret = queue.front();
    queue.erase(queue.begin());
    return ret;
}

void TopicMessageQueue :: push_back(TopicMessage publication) {
    queue.push_back(publication);
}

bool TopicMessageQueue :: empty() {
    return queue.empty();
}