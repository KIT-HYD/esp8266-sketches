/**
   ESP8266 sensor network node

   Needs a omega-logserver at OMEGA_IP with port 5555 open

*/

#include <Arduino.h>
#include <Wire.h>

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
String DEV_ID = "dev-esp8266-2";
String DEV_NAME = "Awesome Soil Moisture";
int INTERVAL = 5*60*1000;


// initialize variables for readings
float temperature;
int capacitance;

// Functions
// write to chrip
void writeI2CRegister8bit(int addr, int value) {
  Wire.beginTransmission(addr);
  Wire.write(value);
  Wire.endTransmission();
}

// read from chirp
unsigned int readI2CRegister16bit(int addr, int reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  delay(20);
  Wire.requestFrom(addr, 2);
  unsigned int t = Wire.read() << 8;
  t = t | Wire.read();
  return t;
}

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

String buildJsonPayload(int capacitance, float temperature) {
  // create dev object
  String dev = "{\"dev_id\": \"" + DEV_ID + "\", \"name\": \"" + DEV_NAME + "\"}";
  String payload = "{\"capacitance\": " + String(capacitance) +
    ", \"temperature\": " + String(temperature) + ", \"dev\": " + dev + "}";

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
  USE_SERIAL.print("Start test script");

  // start chrip
  Wire.begin();
  writeI2CRegister8bit(0x20, 6); // reset chirp;
}

void loop() {
  // put your main code here, to run repeatedly:

  if (joinAP()) {
    USE_SERIAL.println("[WiFi] connected.");

    int s;
    String ack = request_get("acknowledge", s);
    if (ack == "1") {
      USE_SERIAL.println("acknowledged");

      // make the sensor readings here
      capacitance = readI2CRegister16bit(0x20, 0);
      temperature = readI2CRegister16bit(0x20, 5) / 10.0;
      
      String payload = buildJsonPayload(capacitance, temperature);
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
      USE_SERIAL.println("can't reach omega-logserver");
    }
    
  } else {
    USE_SERIAL.println("[WiFi] not connected.");
  }
  delay(INTERVAL);
}
