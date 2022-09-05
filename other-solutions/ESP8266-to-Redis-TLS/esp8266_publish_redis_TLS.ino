#include <Redis.h>
#include <ESP8266WiFi.h>

#define WIFI_SSID "wifi_ssid"
#define WIFI_PASSWORD "wifi_pwd"

#define REDIS_ADDR "redis_host"
#define REDIS_PORT 6380 
#define REDIS_PASSWORD "redis_pwd"

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to the WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250); 
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  WiFiClientSecure redisConn;
  redisConn.setInsecure();

  if (!redisConn.connect(REDIS_ADDR, REDIS_PORT)) {
    Serial.println("Failed to connect to the Redis server!");
    return;
  }

  Redis redis(redisConn);
  auto connRet = redis.authenticate(REDIS_PASSWORD);
  if (connRet == RedisSuccess) {
    Serial.println("Connected to the Redis server!");
  }
  else {
    Serial.printf("Failed to authenticate to the Redis server! Errno: %d\n", (int)connRet);
    return;
  }

  Serial.print("Publish to topic \"balance-1\": ");
  if (redis.publish("balance-1", "123")) { // we write the value captured by the hx711 here
    Serial.println("ok!");
  }
  else {
    Serial.println("err!");
  }

  redisConn.stop();
  Serial.print("Connection closed!");
}

void loop() {
  // do nothing
}
