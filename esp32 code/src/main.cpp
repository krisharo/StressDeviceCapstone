#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <TickTwo.h>
#include <string.h>
#include <time.h>
#include "refs.h" // contains ca certificate for https post

// function declarations
void readNLog();
void valueRead();

// network & API credentials
const char* ssid     = "OnePlus 7T";
const char* password = "d22f3117dc9e";
const char*  server = "web.engr.oregonstate.edu";  // Server URL
const int port = 443;
String apiKeyValue = "tPmAT5Ab3j7F9"; // API key for communicating with serverName

TickTwo timer1(readNLog, 1000*40); // once, every 40s
TickTwo timer2(valueRead, 1000*5); // once every 5s
WiFiClientSecure client;


void setup() {
  timer1.start();
  timer2.start();
  Serial.begin(115200);
   WiFi.mode(WIFI_STA); //The WiFi is in station mode. The    other is the softAP mode
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
   }
   Serial.println("");  Serial.print("WiFi connected to: "); Serial.println(ssid);  Serial.println("IP address: ");  Serial.println(WiFi.localIP());
   client.setCACert(ca_cert); //Only communicate with the server if the CA certificates match
   delay(2000);
}

void loop() {
 timer1.update();
 timer2.update();
}

void valueRead(){
   if(count < 6){
    // analogRead here
    Serial.println("here");
    int temp = analogRead(tPin);
    float voltage = temp * 5;
    voltage /= 1024.0;
    temp = (int) (voltage -.5) * 100; 
    int gsr = analogRead(gPin);
    int hr = analogRead(hrPin);
    // store collected values into array
    tempArr[count] = temp;
    gsrArr[count] = gsr;
    hrArr[count] = hr;
    count++;
  }
  else if(count >= 6){ // array is full, calcualate average
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

void readNLog(){
  int conn;
  conn = client.connect(server, port);
  String body = "api_key=" + apiKeyValue + "&btemp=" + String(tAvg)
                          + "&hr=" + String(hAvg)
                          + "&gsr=" + String(gAvg) + "&switch=" + "0";
  int body_len = body.length();
  if (conn == 1) {
      Serial.println(); Serial.print("Sending Parameters...");
      //Request
      client.println("POST /~harokr/post.php HTTP/1.1");
      //Headers
      client.print("Host: "); client.println(server);
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: "); client.println(body_len);
      client.println("Connection: Close");
      client.println();
      //Body
      client.println(body);
      client.println();

      //Wait for server response
      while (client.available() == 0);

      //Print Server Response
      while (client.available()) {
         char c = client.read();
         Serial.write(c);
      }
   } else {
      client.stop();
      Serial.println("Connection Failed");
   }
}