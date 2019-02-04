#include <Arduino.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
#include <WiFiUdp.h>

// Includes from this project
#include "clock.h"
#include "config.h"
#include "display.h"
#include "graphics.h"
#include "mt.h"

// Settings registers the callback
// to prepare for saving new parameters
WiFiManager wifiManager;
Settings config(&wifiManager);

Ticker ticker, clockTicker, wifiTicker;

WiFiUDP Udp;
WiFiUDP ntpUDP;

Clock clock(ntpUDP, TIMEZONE);

Display display;

DisplayParser displayParser(display, clock);

void flipLED()
{
  static bool ledStatus = true;

  ledStatus = !ledStatus;
  digitalWrite(LED_MAIN, ledStatus);
}

// Gets called when WiFiManager enters configuration mode
void configWifiCallback(WiFiManager *pWifiManager)
{
  Serial.println("Entering wifi configuration mode");
  Serial.print("SSID: ");
  Serial.println(pWifiManager->getConfigPortalSSID());
  Serial.print("IP: ");
  //WiFi.softAP("NeoDisplay", "");
  //pWifiManager->startConfigPortal("NeoDisplay");
  Serial.println(WiFi.softAPIP());
  

  wifiTicker.attach(2.0, flipLED); // TODO: Replace for WiFi AP symbol blinking on display
}

void timerCallback()
{
  // Toggle the LED on the NodeMCU
  flipLED();

  
  // TEMP:
  // TODO: Here we will actuate depending on the mode
  
  // Repaint the screen with new clock update
  //plotClock(clock, display);
  display.draw();
}

void clockISR()
{
  clock.tick();
  plotClock(clock, display);
  //Serial.printf("%d:%d:%d\n", clock.getTime().hour, clock.getTime().minute, clock.getTime().second);
}

void setup()
{
  pinMode(LED_MAIN, OUTPUT);
  digitalWrite(LED_MAIN, HIGH);
  Serial.begin(115200);
  Serial.println("Booting");
  
  /* WiFi Setup */
  WiFi.mode(WIFI_STA);
  //config.resetWiFi(); // Uncomment to reset wifiManager Settings
  // If "setup" is called then settings will need to be saved
  wifiManager.setAPCallback(configWifiCallback);
  if (!wifiManager.autoConnect("NeoDisplay")) 
  {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  Serial.println("Connected! :)");
  wifiTicker.detach();
  config.saveOnDemand(); // Will save if there is need

  /* Read EEPROM */
  config.load();
  display.setMode((Display::Mode)config.display.mode);
  config.print();

  Udp.begin(config.network.port);

  clock.begin();
  clock.update();

  String hostname = "NeoDisplayOTA";
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();

  Serial.println(WiFi.localIP());

  // Checking for OTA upgrade before turning on Interrupts
  // Not tested if this is possible to time
  for (int i = 0; i < 100; i++)
  {
    ArduinoOTA.handle();
  }

  digitalWrite(LED_MAIN, HIGH);

  display.begin();
  display.setBrightness(15);

  display.test();
  display.disco();
  display.clear();
  
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
}
