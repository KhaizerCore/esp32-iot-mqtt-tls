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
using namespace std;

#define LED_BUILTIN 2

#define MQTT_PACKET_SIZE 4096

const char jsonBoardData[] PROGMEM = R"=====(
{
  "license_key": "347848d2-757c-4861-a3e5-c8e12e3c538d",
  "device_setup": [
    {
      "TYPE": "VARIABLE",
      "VARIABLE_NAME": "virtual switch",
      "NAME": "Switch",
      "PIN": -1,
      "CODE": "SWITCH",
      "VALUE": true,
      "VALUE_TYPE": "BOOL",
      "TOPIC_ID": "7ffa7884-b40b-4ac5-9324-54f36c038192"
    },
    {
      "TYPE": "INPUT",
      "VARIABLE_NAME": "",
      "NAME": "DHT22",
      "PIN": 2,
      "CODE": "DHT22",
      "VALUE": 0,
      "VALUE_TYPE": "float",
      "TOPIC_ID": "1609c623-9387-4eb2-b6e4-af1b9a15061c"
    },
    {
      "TYPE": "OUTPUT",
      "VARIABLE_NAME": "",
      "NAME": "Luz da lampada",
      "PIN": 5,
      "CODE": "LED",
      "VALUE": true,
      "VALUE_TYPE": "BOOL",
      "TOPIC_ID": "a22ab6e6-6ff1-4164-af49-83a372db5e2a"
    }
  ]
}
)=====";

Certificates certificates;

PublicationQueue publicationQueue;

//WiFiClient espClient;
WiFiClientSecure net;
PubSubClient client = PubSubClient(net);

DigitalOutput builtInLED(LED_BUILTIN, 1, 0, true);
DigitalOutput whiteLED(5, 1, 0, true);

Lock mqttLock;

time_t now;

unsigned long lastMillis = 0;

String licenseKey = "347848d2-757c-4861-a3e5-c8e12e3c538d";


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
  
  client.subscribe("inTopic");

  //client.publish("board/connected", jsonBoardData);
  client.publish("hello", jsonBoardData);
}

void reconnectMQTT(){
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client";
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
  }
}


void controlLoop(void * parameter){
  while (true) {
      StaticJsonDocument<4000> doc;
      doc["sensor"] = "gps";
      doc["time"] = 1351824120;

      // Add an array.
      //
      JsonArray data = doc.createNestedArray("data");
      data.add(48.756080);
      data.add(2.302038);
      data.add(48.756080);
      data.add(2.302038);
     
      
      //doc["data"]=data;
      // Generate the minified JSON and send it to the Serial port.
      //
      char out[4000];
      int b =serializeJson(doc, out);
      Serial.print("bytes = ");
      Serial.println(b,DEC);

      

      String topic = "hello";
      String message = String(out);

    publicationQueue.push(Publication(topic, message));

    Serial.println("Added publication to queue");
    Serial.println("Queue empty: "+String(publicationQueue.empty()));

    vTaskDelay(5000 / portTICK_PERIOD_MS);  // wait for a second  
  }     
}

void mqttClientLoop(void * parameter){
  while (true) {
    client.loop();

    vTaskDelay(10 / portTICK_PERIOD_MS);  // wait for 10ms 

    if (!client.connected()) {
      reconnectMQTT();
    }
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
  net.setCACert(certificates.emqx);
  
  client.setServer("broker.emqx.io", 8883);

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
    mqttLock.release();
    //vTaskDelay(5 / portTICK_PERIOD_MS);  // wait for a second  
  }
}

void setup() {

  builtInLED.begin();
  whiteLED.begin();

  Serial.begin(9600);

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
