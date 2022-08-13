using namespace std;
#include <Arduino.h>
#include <FreeRTOS.h>
#include "./classes/DigitalOutput.h"
#include "WiFi.h"
#include <WiFiClientSecure.h>
//#include <MQTT.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <vector>
#include <time.h>


#define LED_BUILTIN 2


DigitalOutput builtInLED(LED_BUILTIN, 1, 10, true);
WiFiClientSecure net;
PubSubClient client(net);
time_t now;


unsigned long lastMillis = 0;

const char ca_cert[] PROGMEM = R"=====(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)=====";

const char serverAddr[] PROGMEM = "2eddfd9c7eba4f5a8fa6cc3d402240e3.s1.eu.hivemq.cloud";
const int serverPort = 8883;

const char licenseKey[] PROGMEM = "347848d2-757c-4861-a3e5-c8e12e3c538d";

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

String getBoardData(){

  const int jsonSetupSize = 1024;

  DynamicJsonDocument setup_1(jsonSetupSize);
  setup_1['TYPE'] = "VARIABLE";
  setup_1['VARIABLE_NAME'] = "virtual switch";
  setup_1['NAME'] = "Switch";
  setup_1['PIN'] = -1;
  setup_1['CODE'] = "SWITCH";
  setup_1['VALUE'] = true;
  setup_1['VALUE_TYPE'] = "BOOL";
  setup_1['TOPIC_ID'] = "7ffa7884-b40b-4ac5-9324-54f36c038192";
  //
  char serialized_setup_1[jsonSetupSize];
  serializeJson(setup_1, serialized_setup_1);

  DynamicJsonDocument setup_2(jsonSetupSize);
  setup_2['TYPE'] = "INPUT";
  setup_2['VARIABLE_NAME'] = "";
  setup_2['NAME'] = "DHT22";
  setup_2['PIN'] = 2;
  setup_2['CODE'] = "DHT22";
  setup_2['VALUE'] = 0.0;
  setup_2['VALUE_TYPE'] = "float";
  setup_2['TOPIC_ID'] = "1609c623-9387-4eb2-b6e4-af1b9a15061c";
  //
  char serialized_setup_2[jsonSetupSize];
  serializeJson(setup_2, serialized_setup_2);

  DynamicJsonDocument setup_3(jsonSetupSize);
  setup_3['TYPE'] = "OUTPUT";
  setup_3['VARIABLE_NAME'] = "";
  setup_3['NAME'] = "Luz da lampada";
  setup_3['PIN'] = 5;
  setup_3['CODE'] = "LED";
  setup_3['VALUE'] = true;
  setup_3['VALUE_TYPE'] = "BOOL";
  setup_3['TOPIC_ID'] = "a22ab6e6-6ff1-4164-af49-83a372db5e2a";
  //
  char serialized_setup_3[jsonSetupSize];
  serializeJson(setup_3, serialized_setup_3);

  const int jsonBoardSize = 256 + jsonSetupSize * 3;
  DynamicJsonDocument boardData(jsonBoardSize);

  boardData['license_key'] = licenseKey;
  boardData['device_setup'][0] = serialized_setup_1;
  boardData['device_setup'][1] = serialized_setup_2;
  boardData['device_setup'][2] = serialized_setup_3;
  //
  char serializedBoardData[jsonBoardSize];
  serializeJson(boardData, serializedBoardData);

  return serializedBoardData;
}

const String boardData = getBoardData();


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

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void printTopicSubscribed(String topic){
  Serial.println('Subscribed to topic: ' + topic);
}

void printTopicPublished(String topic, String payload){
  Serial.println('Published to topic: ' + topic);
  Serial.println('Payload: ' + payload);
}

void onClientConnected(){
  Serial.println("Connected");
  
  // Subscribe to its topics on server
  String topic1 = 'server' + licenseKey + '/setup/' + '7ffa7884-b40b-4ac5-9324-54f36c038192';
  client.subscribe(topic1.c_str());
  printTopicSubscribed(topic1);  

  String topic2 = 'server' + licenseKey + '/setup/' + '1609c623-9387-4eb2-b6e4-af1b9a15061c';
  client.subscribe(topic2.c_str());
  printTopicSubscribed(topic2);  

  String topic3 = 'server' + licenseKey + '/setup/'  + 'a22ab6e6-6ff1-4164-af49-83a372db5e2a';
  client.subscribe(topic3.c_str());
  printTopicSubscribed(topic3);  

  // Once connected, publish an announcement...
  client.publish("board/connected", getBoardData().c_str());
  printTopicPublished("board/connected", getBoardData());
}

void reconnect(){
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
      //onClientConnected();
      Serial.println("Connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}

void setup() {
  builtInLED.begin();

  Serial.begin(9600);

  delay(1000);

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

  // mqtts things
  net.setCACert(ca_cert);
  client.setServer(serverAddr, serverPort);
  client.setCallback(callback);
  
  reconnect();
}

void loop() {
  
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    reconnect();
  }
  

  Serial.println("Publishing hello world: ");
  client.publish("/hello", "world");

  

  vTaskDelay(1000 / portTICK_PERIOD_MS);  // wait for a second       
}
