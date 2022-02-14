#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

#include <TickTwo.h>

// Replace with your network credentials
const char* ssid     = "OnePlus 7T";
const char* password = "d22f3117dc9e";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://example.com/post-esp-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode (LED_BUILTIN, OUTPUT);
  Serial.print("ready");

  // WiFi.begin(ssid, password);
  // Serial.println("Connecting");
  // while(WiFi.status() != WL_CONNECTED) { 
  //   delay(500);
  //   Serial.print("trying to conect...");
  // }
  // Serial.println("");
  // Serial.print("Connected to WiFi network with IP Address: ");
  // Serial.println(WiFi.localIP());

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  Serial.print("yo\n");
  delay(1000);
}