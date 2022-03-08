/**
 * @file main.cpp
 * @author Kris Haro (harokr@oregonstate.edu)
 * @brief Firmware used for the wearable stress detection device
 * @version 0.3
 * @date 2022-02-26
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
  #include <WiFiClientSecure.h> 
  #include <ESP8266WebServer.h>
  #include <ESP8266HTTPClient.h>
#endif
#include <TickTwo.h>
#include <string.h>
#include <time.h>
#include "refs.h"

void readNLog();
void valueRead();

//--- Wi-Fi & SQL variables ---
// Wifi netowrk credentials
const char* ssid     = "OnePlus 7T";
const char* password = "d22f3117dc9e";

// destined domain name
const char* host = "web.engr.oregonstate.edu";
String apiKeyValue = "tPmAT5Ab3j7F9"; // API key for communicating with serverName
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80
const char fingerprint [] PROGMEM = "3D 98 A0 26 C8 EA ED 88 41 60 3F 00 C2 1F E1 80 08 E5 34 AB";

//--- Ticker variables for time tracking
TickTwo timer1(readNLog, 1000*40); // once, every 40s
TickTwo timer2(valueRead, 1000*5); // repeat every 5s
int count = 0; // used for array counting



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("ready");
  timer1.start();
  //timer2.start();

  //connect to Wi-Fi network
  WiFi.mode(WIFI_OFF);
  delay(50);
  WiFi.mode(WIFI_STA);
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


void valueRead(){
  if(count < 6){
    srand(time(NULL));
    // generate typical values to be collected
    int temp = rand() % 30 + 20;
    int gsr = rand() % 200 + 140;
    int hr = rand() % 100 + 60;
    // store collected values into array
    tempArr[count] = temp;
    gsrArr[count] = gsr;
    hrArr[count] = hr;
    count++;
  }
  else{ // array is full, calcualate average
    Serial.println("data is ready for transmission");
    int sumT = 0, sumG = 0, sumH = 0;
    for(int i = 0; i < 6; i++){
      sumT += tempArr[i];
      sumG += gsrArr[i];
      sumH += hrArr[i];
    }
    tAvg = sumT / 6;
    hAvg = sumH / 6;
    gAvg = sumG / 6;
  }
  
}
// this function will tick every 60 seconds, where data will be logged
// to the SQL database
void readNLog(){
  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(10000); // 15 Seconds
  delay(500);
  
  Serial.print("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
      delay(100);
      Serial.print(".");
      r++;
  }
  if(r==30) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected to web");
  }

  //POST Data
  String Link = "/~harokr/post.php";

  Serial.print("requesting URL: ");
  Serial.println(host+Link);
  String httpRequestData = "api_key=" + apiKeyValue + "&btemp=" + String(tAvg)
                          + "&hr=" + String(hAvg)
                          + "&gsr=" + String(gAvg) + "&switch=" + "1";

  httpsClient.print(String("POST ") + Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded"+ "\r\n" +
               "Content-Length: " + String(httpRequestData.length()) + "\r\n\r\n" +
               httpRequestData + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
                  
  while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  Serial.println("reply was:");
  Serial.println("==========");
  String line;
  while(httpsClient.available()){        
    line = httpsClient.readStringUntil('\n');  //Read Line by Line
    Serial.println(line); //Print response
  }
  Serial.println("==========");
  Serial.println("closing connection");
  count = 0;
}