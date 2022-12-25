#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>

const char* ssid     = *****; // Replace with your ssid
const char* password = *****; // your password

const int xPin = 35; // Connect your analog stick second pin resistor
const int yPin = 34; // Connect your analog stick second pin resistor 

int xPosition = 0;
int yPosition = 0;
int xValueDown = 0;
int xValueUp = 0;
int yValueLeft = 0;
int yValueRight = 0;


AsyncWebServer server(80); // Create object on port 80


AsyncEventSource events("/events"); // Create an Event Source on /events


JSONVar readings; // Json Variable to Stick Readings

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 60;

String getStickReadings(){
  xPosition = analogRead(xPin);
  yPosition = analogRead(yPin);
    
  if (xPosition < 1700){
    xValueDown = map(xPosition, 0, 1700, 4000, 1700); // Range between 1700 - 1860 is not taken to avoid drift 
  }else if ((xPosition >= 1700) && (xPosition <= 1860)){
    xValueDown = 0;
    xValueUp = 0;
  }else {
    xValueUp = xPosition;
  }
  if (yPosition < 1700){
    yValueLeft = map(yPosition, 0, 1700, 4000, 1700);
  }else if ((yPosition >= 1700) && (yPosition <= 1860)){
    yValueLeft = 0;
    yValueRight = 0;
  }else {
    yValueRight = yPosition;
  }
  readings["xDown"] = String(xValueDown);
  readings["xUp"] = String(xValueUp);
  readings["yLeft"] = String(yValueLeft);
  readings["yRight"] = String(yValueRight);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

const int ledPin1 = 17;

void setup() {
  Serial.begin(115200);
  
  initWiFi();
  initSPIFFS();
 // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.serveStatic("/", SPIFFS, "/");

  // Request for the latest stick readings
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = getStickReadings();
    request->send(200, "application/json", json);
    json = String();
  });

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  // Start server
  server.begin();
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
}

void loop() {
  
  if ((millis() - lastTime) > timerDelay) {
    // Send Events to the client with the Stick Readings Every 60 milliseconds
    events.send("ping",NULL,millis());
    events.send(getStickReadings().c_str(),"new_readings" ,millis());
    lastTime = millis();
  }
}
