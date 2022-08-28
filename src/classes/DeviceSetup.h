#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>
using namespace std;

#ifndef DEVICESETUP_H
#define DEVICESETUP_H

class DeviceSetup {       // The class
    private:             // Access specifier
        

    public:
        DeviceSetup(String, vector<DynamicJsonDocument>); 

        String license_key;        
        vector<DynamicJsonDocument> setupArray;// = {setup1(), setup2(), setup3()};

        void addComponentSetup(DynamicJsonDocument);
        DynamicJsonDocument getBoardDataObject();
        String getBoardDataJson();
        int setupQuantity();
        String getTopicID(int);
        template <typename T> void setSetupValue(T, int);
        String getJsonBoardPartialData(int);
        String getTopicToBePublicated(int);
        String getTopicToSubscribeOnConnectedEvent(int);
};

#endif