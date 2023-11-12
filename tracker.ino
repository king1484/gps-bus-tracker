#define BLYNK_TEMPLATE_ID "templateId"
#define BLYNK_TEMPLATE_NAME "templateName"
#define BLYNK_AUTH_TOKEN "authToken"

#define BLYNK_PRINT Serial

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <ArduinoJson.h>

SoftwareSerial gpsSerial(4, 5);
TinyGPSPlus gps;

char ssid[] = "wifiname";
char pass[] = "password";

BlynkTimer timer;

void myTimerEvent()
{
  time_t epochTime = time(nullptr);
  DynamicJsonDocument jsonDocument(128);
  jsonDocument["last_online"] = epochTime;
  String jsonString;
  serializeJson(jsonDocument, jsonString);
  Blynk.virtualWrite(V1, jsonString);
}

void setup()
{
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  configTime(0, 0, "pool.ntp.org");
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  while(gpsSerial.available() > 0){
    // Serial.write(gpsSerial.read());
    if (gps.encode(gpsSerial.read())) {
        if(gps.location.isValid()){
            double lat = gps.location.lat();
            double lng = gps.location.lng();
            int sat = gps.satellites.value();
            
            //debug
            Serial.print("Latitude: ");
            Serial.println(lat);
            Serial.print("Longitude: ");
            Serial.println(lng);
            //debug

            time_t epochTime = time(nullptr);
            DynamicJsonDocument jsonDocument(128);
            jsonDocument["lat"] = lat;
            jsonDocument["lng"] = lng;
            jsonDocument["sat"] = sat;
            jsonDocument["last_updated"] = epochTime;
            String jsonString;
            serializeJson(jsonDocument, jsonString);
            Blynk.virtualWrite(V0, jsonString);
        }
    }
  }
}
