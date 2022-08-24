#include "PublicationQueue.h"

PublicationQueue :: PublicationQueue () {
    this -> publicationQueue = vector<Publication>();
};


Publication PublicationQueue :: front(){
    return publicationQueue.front();
}

Publication PublicationQueue :: pop_front(){
    Publication ret = publicationQueue.front();
    publicationQueue.erase(publicationQueue.begin());
    return ret;
}

void PublicationQueue :: push(Publication publication) {
    publicationQueue.push_back(publication);
}

bool PublicationQueue :: empty() {
    return publicationQueue.empty();
}