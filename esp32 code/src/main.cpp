#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <TickTwo.h>
#include <string.h>
#include "refs.h" // contains ca certificate for https post

// ------- MCU CONNECTIONS ----------
// HR sensor         GPIO36/VP
// Gsr sensor       GPIO39/VN
// Temp sensor        GPIO34/D34
// User switch       GPIO21/D2

// function declarations
void readNLog();
void valueRead();

// network & API credentials
const char* ssid     = "OnePlus 7T";
const char* password = "d22f3117dc9e";
const char*  server = "web.engr.oregonstate.edu";  // Server URL
const int port = 443;
String apiKeyValue = "tPmAT5Ab3j7F9"; // API key for communicating with serverName


// These timers execute the respective function at given intervals,
// readNLog is for https transmission, valueRead is for sensor readings
TickTwo timer1(readNLog, 1000*60); // once, every 40s
TickTwo timer2(valueRead, 250); // once every 250ms
WiFiClientSecure client;


void setup() {
  pinMode(SWITCH_PIN, INPUT_PULLUP); // initialize the pushbutton pin as an pull-up input
  timer1.start(); // initialize timers
  timer2.start();
  Serial.begin(115200); // set baud rate

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

   // Setup wifi connection
   WiFi.mode(WIFI_STA); //The WiFi is in station mode.
   WiFi.begin(ssid, password); // connect to wifi network
   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
   }
   // successful connection
   Serial.println("");  Serial.print("WiFi connected to: "); Serial.println(ssid);  
   Serial.println("IP address: ");  Serial.println(WiFi.localIP());
   client.setCACert(ca_cert); //Only communicate with the server if the CA certificates match

   delay(1000);
}

void loop() {
 //timer1.update();
 timer2.update();
 // update timers, nothing else should happen here since 
 // all operations are based on the ticker functions
}

// this funciton will collect sensor data
// It will be ran every 500ms to detect a heartbeat
// Every second, gsr and temperature are also collected
// after 30 seconds, or 60 iterations, averages and bpm will be
// calculated
void valueRead(){
   // collect analog values
   if(count < 120){
      if(count % 4 == 0){ // enter here every second
         int tempVal = analogRead(tPin);
         float volts = tempVal/1023.0;             // normalize by the maximum temperature raw reading range
         float temp = (volts - 0.5) * 100 ;         //calculate temperature celsius from voltage as per the equation found on the
         int gsr = 0;
         tempArr[count / 4] = (int)temp;
         gsrArr[count / 4] = gsr;
         currentState = digitalRead(SWITCH_PIN);
         lastState = currentState;
      }
      int hr = analogRead(hrPin); // read hr every 250ms
      if(hr > 1995){
         hrArr[count] = 1; // log if beat is detected
      }
      else{hrArr[count] = 0;}
   }
   
   else if(count >= 120){
      //Serial.println("data is ready for transmission");
      int sumT = 0, sumG = 0;
      for(int i = 0; i < 30; i++){
         sumT += tempArr[i];
         sumG += gsrArr[i];
      }
      tAvg = sumT / 30;
      gAvg = sumG / 30;
      int beatCount = 0;
      for(int i = 0; i < 120; i++){
         if(hrArr[i] == 1){
            beatCount++; // count beats detected
         }
      }
      hAvg = (beatCount * 2) - 10; // number of beats in 30s * 2 = bpm
      if(hAvg < 35){ // filter out heart rates that are too low as they are
      // most likely wrong
         hAvg = 0;
      }
   }
   count++;
}

void readNLog(){
  int conn;
  conn = client.connect(server, port);
  String body = "api_key=" + apiKeyValue + "&btemp=" + String(tAvg)
                          + "&hr=" + String(hAvg)
                          + "&gsr=" + String(gAvg) + "&switch=" + String(currentState);
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
      count =0; // reset count
      for(int i = 0; i < 120; i++){
         hrArr[i] = 0; // reset beat array
      }
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