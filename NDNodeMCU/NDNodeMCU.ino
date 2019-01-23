#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Ticker.h>

// Includes from this project
#include "display.h"
#include "clock.h"
#include "mt.h"

Ticker ticker, clockTicker;

WiFiUDP Udp;
WiFiUDP ntpUDP;

Clock clock(ntpUDP, TIMEZONE);

Display display;

DisplayParser displayParser(display);

volatile unsigned long next;

void flipLED()
{
  static bool ledStatus = true;

  ledStatus = !ledStatus;
  digitalWrite(LED_MAIN, ledStatus);
}

void timerCallback()
{
  flipLED();
  Clock::MyTime time = clock.getTime();

  //display.setRowColor(time.hour, 255 << 8);
  display.clear();
  display.setRowColor(time.second % WIDTH, 0, random(0, 255), random(0, 255));
  display.setRowColor(time.minute % WIDTH, 255, 0, 255);
  display.setRowColor(time.hour, random(0, 255), 255, 255);
  display.update();
}

void clockISR()
{
  clock.tick();
  Serial.printf("%d:%d:%d\n", clock.getTime().hour, clock.getTime().minute, clock.getTime().second);
}

void setup()
{
  pinMode(LED_MAIN, OUTPUT);
  digitalWrite(LED_MAIN, HIGH);
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.hostname("NeoDisplay");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    flipLED();
    Serial.print(".");
  }
  digitalWrite(LED_MAIN, LOW);
  Serial.println(" connected");

  Udp.begin(UDP_PORT);

  clock.begin();
  clock.update();

  String hostname = "NeoDisplayOTA";
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();

  Serial.println(WiFi.localIP());

  display.begin();
  display.setBrightness(15);

  display.test();
  display.disco();
  display.clear();

  // Checking for OTA upgrade before turning on Interrupts
  // Not tested if this is possible to time
  for (int i = 0; i < 100; i++)
  {
    ArduinoOTA.handle();
  }
  
  ticker.attach_ms(LOOPTIME, timerCallback);
  clockTicker.attach_ms(1e3, clockISR);
}

void loop()
{
  ArduinoOTA.handle();

  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    for (int i = 0; i < packetSize; i++)
    {
      uint8_t newByte = Udp.read();
      displayParser.parse(newByte);
    }
  }
  //clock.update();
}
