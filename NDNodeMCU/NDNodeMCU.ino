#include <Arduino.h>
#include <ArduinoOTA.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include "display.h"
#include "mt.h"

const char* ssid = "****";
const char* password = "****";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;

Display display;

DisplayParser displayParser(display);

void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.hostname("pixeldisplay");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);

  String hostname = "NeoDisplayOTA";
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();

  Serial.println(WiFi.localIP());

  display.begin();

  // TEST 
  std::vector<Display::Pixel> pixels(4);
  pixels[0].index = 0;
  pixels[0].r = 255;
  
  pixels[1].index = 1;
  pixels[1].g = 255;
  
  pixels[2].index = 2;
  pixels[2].b = 255;
  
  pixels[3].index = 3;
  pixels[3].r = 255;
  pixels[3].g = 255;
  pixels[3].b = 255;
  display.setPixels(pixels);
  display.setBrightness(15);
  display.test();
  display.clear();
}

void loop()
{
  ArduinoOTA.handle();

  int packetSize = Udp.parsePacket();
  if (packetSize)
  for (int i = 0; i < packetSize; i++)
  {
    uint8_t newByte = Udp.read();
    displayParser.parse(newByte);
  }
}
