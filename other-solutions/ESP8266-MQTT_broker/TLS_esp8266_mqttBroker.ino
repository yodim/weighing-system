#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = D1;
const int LOADCELL_SCK_PIN = D2;

const char *ssid = ""; // Enter your WiFi name
const char *password = "";  // Enter WiFi password// MQTT Broker
const char *mqtt_broker = ""; // Enter your WiFi or Ethernet IP
const char *onConnect_topic = "connected";
const char *balance_topic = "balance/1";
const int mqtt_port = 1883;
int wifi_retries = 20;

HX711 scale;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  scale.tare();
  Serial.print("taring done.");
}

void setup() {
  // Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    if (wifi_retries > 0 ) {
      delay(500);
      Serial.println("Connecting to WiFi..");
      wifi_retries--;
    }
    else {
      ESP.restart();
    }
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());

    Serial.printf("\nThe client %s connects to mqtt broker\n", client_id.c_str());

    if (client.connect("Balance1")) {
      Serial.println("Public mqtt broker connected");
      client.subscribe("tare/1");
      client.publish(onConnect_topic, "Balance1 connected !");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
      ESP.restart();
    }
  }

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("taring the scale... remove any weight from the scale"); 
  delay(3000);
  scale.set_scale(66.72);      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();                 // reset the scale to 0
  Serial.println("Taring done.");
  Serial.println("Readings:");
}


void loop() {
  if( WiFi.status() != WL_CONNECTED || !client.connected()) {
    ESP.restart();
  }
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  char weight[10];
  sprintf(weight, "%d", int(scale.get_units(10)));
  Serial.println(weight);
  client.publish(balance_topic, weight);
  // put the ADC in sleep mode
  delay(1000);
  client.loop();
}