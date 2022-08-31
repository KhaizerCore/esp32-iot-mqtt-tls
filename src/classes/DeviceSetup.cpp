#include "DeviceSetup.h"

#define JSON_SETUP_SIZE 1024

DeviceSetup :: DeviceSetup(String license_key, vector<DynamicJsonDocument> setupArray) {
    this->license_key = license_key;
    this->setupArray = setupArray;
}

void DeviceSetup:: addComponentSetup(DynamicJsonDocument setup){
    this -> setupArray.push_back(setup);
}

DynamicJsonDocument DeviceSetup:: getBoardDataObject(){
    const int jsonBoardDataSize = 256 + JSON_SETUP_SIZE * this->setupArray.size();
    DynamicJsonDocument boardData(jsonBoardDataSize);
    boardData["license_key"] = this->license_key;
    for (int i = 0; i < setupArray.size(); i++){
        boardData["device_setup"][i] = setupArray.at(i);
    }
    return boardData;
}

String DeviceSetup :: getBoardDataJson(){
  String serializedBoardData;  
  serializeJson(getBoardDataObject(), serializedBoardData);
  return serializedBoardData;
}

int DeviceSetup :: setupQuantity(){
    return this->setupArray.size();
}

String DeviceSetup :: getTopicID(int setup_idx){
    return this -> setupArray.at(setup_idx).getMember("TOPIC_ID");
}

template <typename T> void DeviceSetup :: setSetupValue(T value, int setup_idx){
    this -> setupArray.at(setup_idx)["VALUE"] = value;
}
template void DeviceSetup::setSetupValue<int>(int, int);

String DeviceSetup :: getJsonBoardPartialData(int setup_idx){
    const int jsonSize = 256 + JSON_SETUP_SIZE;
    DynamicJsonDocument boardPartialData(jsonSize);

    boardPartialData["license_key"] = this -> license_key;
    boardPartialData["device_setup"] = this -> setupArray.at(setup_idx);

    String serializedBoardPartialData;  
    serializeJson(boardPartialData, serializedBoardPartialData);
    return serializedBoardPartialData;
}

String DeviceSetup :: getTopicToBePublicated(int setup_idx) {
    return "board/" + this ->license_key + "/setup/" + this -> getTopicID(setup_idx);
}

String DeviceSetup :: getTopicToSubscribeOnConnectedEvent(int setup_idx) {
    return "server/" + this -> license_key + "/setup/" + this -> getTopicID(setup_idx);
}
