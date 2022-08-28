#include <Arduino.h>
#include <FreeRTOS.h>
#include "./classes/DigitalOutput.h"
#include "./classes/Lock.h"
#include "./classes/PublicationQueue.h"
#include "./classes/Publication.h"
#include "./classes/Certificates.h"
#include "WiFi.h"
#include <WiFiClientSecure.h>
//#include <MQTT.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <vector>
#include <time.h>
#include <DHT.h>

using namespace std;

#define LED_BUILTIN 2
#define MQTT_PACKET_SIZE 4096
#define JSON_SETUP_SIZE 1024
#define DHTPIN 4
#define DHTTYPE DHT11

// Setup do componente 1
DynamicJsonDocument setup1(){
  DynamicJsonDocument setup_1(JSON_SETUP_SIZE);
  setup_1["TYPE"] = "VARIABLE";
  setup_1["VARIABLE_NAME"] = "virtual switch";
  setup_1["NAME"] = "Switch";
  setup_1["PIN"] = -1;
  setup_1["CODE"] = "SWITCH";
  setup_1["VALUE"] = true;
  setup_1["VALUE_TYPE"] = "BOOL";
  setup_1["TOPIC_ID"] = "7ffa7884-b40b-4ac5-9324-54f36c038192";
  return setup_1;
}

// Setup do componente 2
DynamicJsonDocument setup2(){
  DynamicJsonDocument setup_2(JSON_SETUP_SIZE);
  setup_2["TYPE"] = "INPUT";
  setup_2["VARIABLE_NAME"] = "";
  setup_2["NAME"] = "DHT22";
  setup_2["PIN"] = 2;
  setup_2["CODE"] = "DHT22";
  setup_2["VALUE"] = 0.0;
  setup_2["VALUE_TYPE"] = "float";
  setup_2["TOPIC_ID"] = "1609c623-9387-4eb2-b6e4-af1b9a15061c";
  return setup_2;
}

// Setup do componente 3
DynamicJsonDocument setup3(){
  DynamicJsonDocument setup_3(JSON_SETUP_SIZE);
  setup_3["TYPE"] = "OUTPUT";
  setup_3["VARIABLE_NAME"] = "";
  setup_3["NAME"] = "Luz da lampada";
  setup_3["PIN"] = 5;
  setup_3["CODE"] = "LED";
  setup_3["VALUE"] = 0;
  setup_3["VALUE_TYPE"] = "int";
  setup_3["TOPIC_ID"] = "a22ab6e6-6ff1-4164-af49-83a372db5e2a";
  return setup_3;
}

// Licença da placa
String licenseKey = "347848d2-757c-4861-a3e5-c8e12e3c538d";
vector<DynamicJsonDocument> setupArray = {setup1(), setup2(), setup3()};

const int jsonBoardDataSize = 256 + JSON_SETUP_SIZE * setupArray.size();


DynamicJsonDocument getBoardDataObject(){
  DynamicJsonDocument boardData(jsonBoardDataSize);

  boardData["license_key"] = licenseKey;

  for (int i = 0; i < setupArray.size(); i++){
    boardData["device_setup"][i] = setupArray.at(i);
  }

  return boardData;
}

String getBoardData(){
  //char serializedBoardData[jsonBoardSize];
  String serializedBoardData;
  
  serializeJson(getBoardDataObject(), serializedBoardData);

  return serializedBoardData;
}

const String boardData = getBoardData();




Certificates certificates;
PublicationQueue publicationQueue;
WiFiClientSecure net;
PubSubClient client = PubSubClient(net);
DigitalOutput builtInLED(LED_BUILTIN, 1, 0, true);
DigitalOutput whiteLED(5, 1, 0, true);
Lock mqttLock;
time_t now;
DHT dht(DHTPIN, DHTTYPE);

int random_number(int min, int max) //range : [min, max]
{
   static bool first = true;
   if (first) 
   {  
      srand( time(NULL) ); //seeding for the first time only!
      first = false;
   }
   return min + rand() % (( max + 1 ) - min);
}


void callback(char* topic, byte* payload, unsigned int length) {
  mqttLock.acquire();
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
  mqttLock.release();
}

void onConnect(){

  // INITIAL SUBSCRIPTIONS   
  //client.subscribe("inTopic");

  vTaskDelay(10 / portTICK_PERIOD_MS);

  // Subscribe in each server topic related to board's own componentes, in order to receive updates from the web user, trought the middleware
  for (int i = 0; i < setupArray.size(); i++){
    String topic_id = setupArray.at(i).getMember("TOPIC_ID");
    String topic = "server/" + licenseKey + "/setup" + topic_id;
    client.subscribe(topic.c_str());
    Serial.println("subscribing to " + topic);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  vTaskDelay(50 / portTICK_PERIOD_MS);

  // INITIAL PUBLICATIONS
  client.publish("board/connected", boardData.c_str());

  vTaskDelay(50 / portTICK_PERIOD_MS);
}

void reconnectMQTT(){
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-legutaobala";
    String username = "sigiotsystem";
    String password = "sigiotsystem";

    Serial.print("Time:");
    Serial.print(ctime(&now));
    Serial.print("MQTT connecting");
    
    // Attempt to connect
    if (client.connect(clientId.c_str(), username.c_str(), password.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      mqttLock.acquire();
      onConnect();
      mqttLock.release();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}


void controlLoop(void * parameter){
  while (true) {
      // StaticJsonDocument<4000> doc;


      // Lets post the first element of device setup
        setupArray.at(1)["VALUE"] = random_number(-10, 40); // generates random number in port
        
        const int jsonSize = 256 + JSON_SETUP_SIZE;
        DynamicJsonDocument boardPartialData(jsonSize);

        boardPartialData["license_key"] = licenseKey;
        boardPartialData["device_setup"] = setupArray.at(1);

        String serializedBoardPartialData;  
        serializeJson(boardPartialData, serializedBoardPartialData);

        String topic_id = setupArray.at(1)["TOPIC_ID"];
        String topic_test = "board/" + licenseKey + "/setup/" + topic_id;

        publicationQueue.push(Publication(topic_test, serializedBoardPartialData.c_str()));
      //

      // Lets post the first element of device setup
      // const float temp = dht.readTemperature();
      //   Serial.println("dht temp:" + String(temp));
      //   setupArray.at(1)["VALUE"] = temp;
        
      //   //const int jsonSize = 256 + JSON_SETUP_SIZE;
      //   DynamicJsonDocument boardPartialData2(jsonSize);

      //   boardPartialData["license_key"] = licenseKey;
      //   boardPartialData["device_setup"] = setupArray.at(1);

      //   String serializedBoardPartialData2;  
      //   serializeJson(boardPartialData2, serializedBoardPartialData2);

      //   String topic_id2 = setupArray.at(1)["TOPIC_ID"];
      //   String topic_test2 = "board/" + licenseKey + "/setup/" + topic_id2;

      //   publicationQueue.push(Publication(topic_test2, serializedBoardPartialData2.c_str()));
      //

    //   doc["sensor"] = "gps";
    //   doc["time"] = 1351824120;

    //   // Add an array.
    //   //
    //   JsonArray data = doc.createNestedArray("data");
    //   data.add(random_number(0, 100));
    //   data.add(random_number(0, 100));
    //   data.add(random_number(0, 100));
    //   data.add(random_number(0, 100));
     
      
    //   //doc["data"]=data;
    //   // Generate the minified JSON and send it to the Serial port.
    //   //
    //   char out[4000];
    //   int b =serializeJson(doc, out);
    //   Serial.print("bytes = ");
    //   Serial.println(b,DEC);

      

    //   String topic = "hello";
    //   String message = String(out);

    // publicationQueue.push(Publication(topic, message));

    // Serial.println("Added publication to queue");
    //Serial.println("Queue empty: "+String(publicationQueue.empty()));

    vTaskDelay(5000 / portTICK_PERIOD_MS);  // wait for a second  
  }     
}


void connectWifi(){
  //Wifi Connection
  String ssid = "Casa Router 4";
  String password = "Caxambu1942";
  WiFi.begin((const char*)ssid.c_str(), (const char*)password.c_str());
  
  // Print WIFI Info
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(1000);
    }
    Serial.println();
    Serial.print("Connected to ");
    Serial.println(ssid);
}

void adjustTime(){
  // Adjusting time 

  Serial.print("Setting time using SNTP");
  configTime(-5 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < 1510592825) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void mqttSetup(){
  client.setBufferSize(MQTT_PACKET_SIZE);
  // mqtts things
  net.setCACert(certificates.hivemq);
  
  //client.setServer("broker.emqx.io", 8883);

  client.setServer("2eddfd9c7eba4f5a8fa6cc3d402240e3.s1.eu.hivemq.cloud", 8883);

  client.setCallback(callback);
}


void sendPublicationQueue(){
  
  while (!publicationQueue.empty() && client.connected()){
    Publication publication = publicationQueue.front();

    Serial.println("Publishing topic "+publication.getTopic());
    Serial.println("Message: "+publication.getPublication());

    mqttLock.acquire();
    if (client.publish(publication.getTopic().c_str(), publication.getPublication().c_str())){
      publicationQueue.pop_front();

      Serial.println("MQTT message sent successfully");
    }else{
      Serial.println("MQTT message failed to publish");
    }
    vTaskDelay(5 / portTICK_PERIOD_MS);  // wait for 5 ms 
    mqttLock.release();
    vTaskDelay(5 / portTICK_PERIOD_MS);  // wait for 5 ms
  }
}

void setup() {

  Serial.begin(9600);

  // Components initialization
  dht.begin();
  builtInLED.begin();
  whiteLED.begin();
  //

  connectWifi();

  adjustTime();

  mqttSetup();
  
  reconnectMQTT();



  xTaskCreate(
    controlLoop,  /* Task function. */
    "TaskOne",            /* String with name of task. */
    10000,                /* Stack size in bytes. */
    NULL,                 /* Parameter passed as input of the task */
    1,                    /* Priority of the task. */
    NULL
  );                      /* Task handle. */

}

void loop() {  
  client.loop();

  vTaskDelay(10 / portTICK_PERIOD_MS);  // wait for a second  

  if (!client.connected()) {
    reconnectMQTT();
  }

  sendPublicationQueue();       
}
