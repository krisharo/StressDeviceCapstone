/**
 * @file main.cpp
 * @author Kris Haro (harokr@oregonstate.edu)
 * @brief Firmware used for the wearable stress detection device
 * @version 0.2
 * @date 2022-02-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
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
#include 'refs.h'

void readNLog();
void valueRead();

//--- Wi-Fi & SQL variables ---
// Wifi netowrk credentials
const char* ssid     = "OnePlus 7T";
const char* password = "d22f3117dc9e";

// destined domain name
const char* serverName = "https://web.engr.oregonstate.edu/~harokr/post.php";
String apiKeyValue = "tPmAT5Ab3j7F9"; // API key for communicating with serverName

//--- Ticker variables for time tracking
TickTwo timer1(readNLog, 1000*60); // once, every minute
TickTwo timer2(valueRead, 5); // repeat every 5 ms

// --- Variables for data collection
int gsrArr[15]; // hold 15 values to average out
int hrArr[15];
int tempArr[15];
int count = 0;
const int curGsr = A0; // current value on the analog pin
int gsr = 0; // average that will be logged/discarded if it is bad


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("ready");
  timer1.start();
  timer2.start();

  //connect to Wi-Fi network
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print("trying to conect...");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // put your main code here, to run repeatedly:
  timer2.update(); // update timers
  timer1.update();
}

// this function is used to capture the analog inputs
// it will trigger every 5ms
void valueRead(){
  int sensorVal = analogRead(curGsr); // read gsr
  gsrArr[count] = sensorVal;
  count++;
  if(count == 14){ // array is full, take average
    int gSum = 0;
    for(int i = 0; i < count; i++){
      gSum +=gsrArr[i];
    }
    gsr = gSum / 15;
    count = 0;
  }
}

// this function will tick every 60 seconds, where data will be logged
// to the SQL database
void readNLog(){
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&btemp=" + "29"
                          + "&otemp=" + "23" + "&hr=" + "89"
                          + "&gsr=" + "370" + "&switch=" + "1" + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);        
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}