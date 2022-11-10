#include <Arduino.h>
#include <FreeRTOS.h>
#include "DigitalOutput.h"
#include "Lock.h"
#include "TopicMessageQueue.h"
#include "TopicMessage.h"
#include "Certificates.h"
#include "DeviceSetup.h"
//#include "WiFi.h"
#include <WiFiClientSecure.h>
//#include <MQTT.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <vector>
#include <time.h>
#include <DHT.h>
#include <Display.h>

using namespace std;

#define LED_BUILTIN 2
#define EXTERNAL_LED 5
#define MQTT_PACKET_SIZE 4096
#define JSON_SETUP_SIZE 1024
#define DHTPIN 4
#define DHTTYPE DHT11

// Setup do componente 1
DynamicJsonDocument setup1(){
  DynamicJsonDocument setup_1(JSON_SETUP_SIZE);
  setup_1["TYPE"] = "INPUT";
  setup_1["VARIABLE_NAME"] = "";
  setup_1["NAME"] = "Humidity";
  setup_1["PIN"] = 4;
  setup_1["CODE"] = "DHT11";
  setup_1["VALUE"] = 0.0;
  setup_1["VALUE_TYPE"] = "DOUBLE";
  setup_1["TOPIC_ID"] = "7ffa7884-b40b-4ac5-9324-54f36c038192";
  return setup_1;
}

// Setup do componente 2
DynamicJsonDocument setup2(){
  DynamicJsonDocument setup_2(JSON_SETUP_SIZE);
  setup_2["TYPE"] = "INPUT";
  setup_2["VARIABLE_NAME"] = "";
  setup_2["NAME"] = "Temperature";
  setup_2["PIN"] = 4;
  setup_2["CODE"] = "DHT11";
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
  setup_3["NAME"] = "LED";
  setup_3["PIN"] = 5;
  setup_3["CODE"] = "LED";
  setup_3["VALUE"] = false;
  setup_3["VALUE_TYPE"] = "BOOL";
  setup_3["TOPIC_ID"] = "a22ab6e6-6ff1-4164-af49-83a372db5e2a";
  return setup_3;
}

// Setup do componente 4
DynamicJsonDocument setup4(){
  DynamicJsonDocument setup_4(JSON_SETUP_SIZE);
  setup_4["TYPE"] = "VARIABLE";
  setup_4["VARIABLE_NAME"] = "displayText";
  setup_4["NAME"] = "DisplayText";
  setup_4["PIN"] = -1;
  setup_4["CODE"] = "displayText";
  setup_4["VALUE"] = "Initial Text";
  setup_4["VALUE_TYPE"] = "STRING";
  setup_4["TOPIC_ID"] = "7ffa7884-6ff1-4eb2-af49-83a372db5e21";
  return setup_4;
}

String licenseKey = "80c7ebba-762b-4c22-80fa-1bfd31e4535f";
DeviceSetup deviceSetup = DeviceSetup(licenseKey, {setup1(), setup2(), setup3(), setup4()});


//Certificates certificates;
TopicMessageQueue publicationQueue;
TopicMessageQueue subscribedMessageQueue;
WiFiClientSecure net;
PubSubClient client = PubSubClient(net);
DigitalOutput builtInLED(LED_BUILTIN, true);
DigitalOutput externalLED(EXTERNAL_LED, false);
Lock mqttLock;
time_t now;
DHT dht(DHTPIN, DHTTYPE);
Display display = Display();

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


void onMessageReceived(char* topic, byte* payload, unsigned int length) {
  mqttLock.acquire();
  
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  


  String message = "";
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];
  }

  Serial.println(" Payload: " + message);

  subscribedMessageQueue.push_back(
    TopicMessage(String(topic), message)
  );

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
  mqttLock.release();
}

void onConnect(){

  // INITIAL SUBSCRIPTIONS   
  client.subscribe("inTopic");
  vTaskDelay(10 / portTICK_PERIOD_MS);

  // Subscribe in each server topic related to board's own componentes, in order to receive updates from the web user, trought the middleware
  for (int i = 0; i < deviceSetup.setupArray.size(); i++){
    String topic = deviceSetup.getTopicToSubscribeOnConnectedEvent(i);
    client.subscribe(topic.c_str());
    Serial.println("subscribing to " + topic);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  // INITIAL PUBLICATIONS
  client.publish("board/connected", deviceSetup.getBoardDataJson().c_str());

  vTaskDelay(10 / portTICK_PERIOD_MS);
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
    Serial.println(ctime(&now));
    Serial.println("MQTT connecting");
    
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

    if (deviceSetup.setupArray.at(2)["VALUE"]){
      externalLED.turnOn();
    }else{
      externalLED.turnOff();
    }

    vTaskDelay(1 / portTICK_PERIOD_MS);  // wait for a second  
  }     
}

void messageReceivedLoop(void * parameter){
  while (true) {

    if (!subscribedMessageQueue.empty()) {
      TopicMessage publication = subscribedMessageQueue.front();
      
      for (int setup_idx = 0; setup_idx < deviceSetup.setupArray.size(); setup_idx++){
        const String myTopic = deviceSetup.getTopicToSubscribeOnConnectedEvent(setup_idx);
        if (myTopic == publication.getTopic()){
          
          DynamicJsonDocument doc(publication.getMessage().length() * 6);
          DeserializationError err =  deserializeJson(doc, publication.getMessage());
          
          if (err){
            Serial.print("ERROR: ");
            Serial.println(err.c_str());
          }else{
            //Serial.println("Deserialization successfull");
          }

          JsonObject jsonMessage = doc.as<JsonObject>();

          for (JsonPair keyValue : jsonMessage) {
            Serial.println("keyvalue received: " + String(keyValue.key().c_str()));
            
            deviceSetup.setupArray.at(setup_idx)[keyValue.key().c_str()] = keyValue.value();
          }

          // Display update support
          if (setup_idx == 3){
            String text = deviceSetup.setupArray.at(3)["VALUE"];
            display.print(text, 10, 2);
          }
        }
      }

      subscribedMessageQueue.pop_front();
    }


    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
}

void connectWifi(){
  //Wifi Connection
  String ssid = "Galaxy M52 5G";
  String password = "jyil3233";
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
  net.setCACert(hivemqCert);
  
  //client.setServer("broker.emqx.io", 8883);

  client.setServer("2eddfd9c7eba4f5a8fa6cc3d402240e3.s1.eu.hivemq.cloud", 8883);

  client.setCallback(onMessageReceived);
}

void sendPublicationQueue(){
  
  while (!publicationQueue.empty() && client.connected()){
    TopicMessage publication = publicationQueue.front();

    Serial.println("\n\nPublishing topic "+publication.getTopic());
    Serial.println("Message: "+publication.getMessage());

    mqttLock.acquire();
    if (client.publish(publication.getTopic().c_str(), publication.getMessage().c_str())){
      publicationQueue.pop_front();

      Serial.println("MQTT message sent successfully");
    }else{
      Serial.println("MQTT message failed to publish");
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);  // wait for 1 ms 
    mqttLock.release();
    vTaskDelay(1 / portTICK_PERIOD_MS);  // wait for 1 ms
  }
}

void readDHTSensor(void * parameter){
  while(true){
    vTaskDelay(2000 / portTICK_PERIOD_MS);   

    float humidity = dht.readHumidity();
    deviceSetup.setSetupValue<float>(humidity, 0); // updates value of setup value with index 0

    float temperature = dht.readTemperature();        
    deviceSetup.setSetupValue<float>(temperature, 1); // updates value of setup value with index 1

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    if (!isnan(humidity) & !isnan(temperature)){
      int setup_idx = 0;       
      publicationQueue.push_back(
        TopicMessage(
          deviceSetup.getTopicToBePublished(setup_idx), 
          deviceSetup.getJsonBoardPartialData(setup_idx).c_str()
        )
      );

      setup_idx++;
      publicationQueue.push_back(
        TopicMessage(
          deviceSetup.getTopicToBePublished(setup_idx), 
          deviceSetup.getJsonBoardPartialData(setup_idx).c_str()
        )
      );
    }
  }
}

void setup() {

  Serial.begin(9600);

  // Components initialization
  dht.begin();
  builtInLED.begin();
  externalLED.begin();

  display.begin();
  display.init();
  //

  delay(50);
  String text = deviceSetup.setupArray.at(3)["VALUE"];
  display.print(text, 10, 2);

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

  xTaskCreate(
    messageReceivedLoop,  /* Task function. */
    "TaskTwo",            /* String with name of task. */
    10000,                /* Stack size in bytes. */
    NULL,                 /* Parameter passed as input of the task */
    1,                    /* Priority of the task. */
    NULL
  );                      /* Task handle. */

  xTaskCreate(
    readDHTSensor,  /* Task function. */
    "TaskThree",            /* String with name of task. */
    2000,                /* Stack size in bytes. */
    NULL,                 /* Parameter passed as input of the task */
    2,                    /* Priority of the task. */
    NULL
  );

}

void loop() {  
  client.loop();

  vTaskDelay(10 / portTICK_PERIOD_MS);  // wait for 10ms

  if (!client.connected()) {
    reconnectMQTT();
  }

  sendPublicationQueue();       
}
