#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

const char* ssid     = "WIFI_SSID";     // your network SSID (name of wifi network)
const char* password = "WIFI_PASSWORD";               // your network password

WiFiClientSecure client;
WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);

      // send message to server when Connected
      //webSocket.sendTXT("Connected");
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);

      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}


void setup() {
  Serial.begin(9600);
  delay(100);

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);



  // server address, port and URL
  webSocket.beginSSL("server_addr", 8001, "/wss/set-balance/1/" );
  webSocket.setExtraHeaders("Origin: http://127.0.0.1"); // add Origin header to authorize connection

  // event handler
  webSocket.onEvent(webSocketEvent);

  // use HTTP Basic Authorization this is optional remove if not needed
 // webSocket.setAuthorization("username", "password");

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

}

void loop() {
  char poids[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //initialize the weight variable
  int int_tmp;
  char char_tmp;

  while (Serial.available()) {
    int_tmp = Serial.read();
    // parsing the read data
    if ( int_tmp == '=') {
      int i = 0;
      while (Serial.available() && i < 10) {
        int_tmp = Serial.read();
        if ( int_tmp != '\n' && int_tmp != '\r' ) {
          poids[i] = int_tmp;
        } else {
          break;
        }
        i++;
      }
    }
  }
  if ( poids[0] ) {
    // formatting the data as json to send to the server
    String msg = "{\"message\": \"" + String(poids) + "\", \"secret\": \"s3cr3t\"}";
    webSocket.sendTXT(msg);
  }
  webSocket.loop();
  delay(100);
}
