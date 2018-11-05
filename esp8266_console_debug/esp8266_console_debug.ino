/**
   ESP8266 sensor network node

   Needs a omega-logserver at OMEGA_IP with port 5555 open

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

// WiFi settings, maybe put EEPROM?
const char* OMEGA_SSID = "Omega-BE81";
const char* OMEGA_PWD = "hackme4life";
String HOST = "http://192.168.3.1:5555/";

// device settings
String DEV_ID = "dev-esp8266-1";
String DEV_NAME = "NodeMCU ESP8266 test device";

// fake measurement settings
const int INTERVAL = 15000;

String request_get(String endpoint, int &statusCode) {
  HTTPClient http;

  http.begin(HOST + endpoint);

  statusCode = http.GET();

  if (statusCode > 0) {
    String payload =  http.getString();
    http.end();
    return payload;
  } else {
    String error = http.errorToString(statusCode);
    http.end();
    return error;
  }
}

String request_put(String endpoint, int &statusCode, String body) {
  HTTPClient http;

  http.begin(HOST + endpoint);

  // set content type
  http.addHeader("Content-Type", "application/json");

  // Put the body
  statusCode = http.PUT(body);

  if (statusCode > 0){
    String response = http.getString();
    http.end();
    return response;
  } else {
    String error = http.errorToString(statusCode);
    http.end();
    return error;
  }
}

String buildJsonPayload(float temperature) {
  // create dev object
  String dev = "{\"dev_id\": \"" + DEV_ID + "\", \"name\": \"" + DEV_NAME + "\"}";
  String payload = "{\"temperature\": " + String(temperature) + ", \"dev\": " + dev + "}";

  return payload;
}

boolean joinAP() {
  WiFi.mode(WIFI_STA);
  
  WiFiMulti.addAP(OMEGA_SSID, OMEGA_PWD);
  delay(1);


  if ((WiFiMulti.run() == WL_CONNECTED)) {
      return true;
  } else {
    return false;
  }
}

void setup() {
  // Open serial console for debugging
  USE_SERIAL.begin(115200);
  USE_SERIAL.println("Start test script");
  USE_SERIAL.println("-----------------");
}

void loop() {
  // put your main code here, to run repeatedly:

  if (joinAP()) {
    USE_SERIAL.print("[WiFi] connected. Search Base Station....");

    int s;
    String ack = request_get("acknowledge", s);
    if (ack == "1") {
      USE_SERIAL.println("acknowledged.");

      // make the sensor readings here
      float temp = random(-100, 400) / 10.0;
      String payload = buildJsonPayload(temp);
      USE_SERIAL.print("Send Payload: ");
      USE_SERIAL.println(payload);

      // send the data to the omega
      String response = request_put("log", s, payload);
      if (response == "201") {
        USE_SERIAL.println("acknowledged.");
      } else {
        USE_SERIAL.println("[ERROR]: " + response);
      }

    } else {
      USE_SERIAL.println("not found.");
    }
    
  } else {
    USE_SERIAL.println("[WiFi] not connected.");
  }
  delay(INTERVAL);
}
