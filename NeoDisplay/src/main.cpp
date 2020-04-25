#include <Arduino.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <Ticker.h>               // https://github.com/esp8266/Arduino/tree/master/libraries/Ticker
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
#include <WiFiUdp.h>

// Includes from this project
#include "clock.h"
#include "config.h"
#include "mydisplay.h"
#include "graphics.h"
#include "mt.h"

enum WiFiStatus {
  CONNECTING = 0,
  CONNECTED,
  AP,
};

WiFiStatus wifiStatus = CONNECTING;

// Settings registers the callback
// to prepare for saving new parameters
Settings* pConfig = Settings::Instance();
WiFiManager* pWiFiManager = pConfig->getWiFiManager();

Ticker mainTicker, clockTicker;
bool mainTaskReady = false;
bool clockTaskReady = false;

WiFiUDP Udp;
WiFiUDP ntpUDP;

Clock myClock(ntpUDP, TIMEZONE);

MyDisplay display;

DisplayParser displayParser(display, myClock);

// For calculation of CPU Load
uint16_t counter = 0;
const uint16_t maxLoad = 46300; // uint16 increments in a sec

//! Ran every LOOPTIME ms
void mainISR()
{
  mainTaskReady = true;
}

//! Ran every s
void clockISR()
{
  #ifdef MEASURE_CPU
    static float mean = 0.0f, peak = 0.0f;
    float usage = float(maxLoad-counter)/(float)maxLoad;
    if (usage > peak) peak = usage;
    mean = (float)mean*0.7 + usage*0.3;
    Serial.printf("CPU usage: %.2f. Peak: %.2f, counter: %d\n", mean, peak, counter);
    counter = 0;
  #endif
  clockTaskReady = true;
}

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
  //pWifiManager->startConfigPortal("NeoDisplay");
  Serial.println(WiFi.softAPIP());
  
  wifiStatus = AP;
}

void mainWorker()
{
  // Toggle the LED on the NodeMCU
  flipLED();

  switch (display.getMode())
  {
  case MyDisplay::Mode::CONNECTING: // Animate WiFi
    if (wifiStatus == CONNECTING)
    {
      plotWiFi(display);
    }
    else
    {  
      plotWiFi(display, true, wifiStatus == CONNECTED);
    }
    break;  
  case MyDisplay::Mode::CLOCK:
    // Repaint the screen with new clock update
    plotClock(myClock, display);
    break;
  case MyDisplay::Mode::NORMAL:
  default:
    break;
  }
  display.draw();
}

void setup()
{
  wifi_station_set_hostname("NeoDisplay");
  pinMode(LED_MAIN, OUTPUT);
  digitalWrite(LED_MAIN, HIGH);
  Serial.begin(115200);
  Serial.println("Booting");
  
  display.begin();
  display.setBrightness(15);
  display.setEffect(MyDisplay::Effect::FADE);

  myClock.begin();
  mainTicker.attach_ms(LOOPTIME, mainISR);
  
  /* WiFi Setup */
  WiFi.mode(WIFI_STA);
  //config.resetWiFi(); // Uncomment to reset wifiManager Settings
  // If "setup" is called then settings will need to be saved
  pWiFiManager->setAPCallback(configWifiCallback);
  if (!pWiFiManager->autoConnect("NeoDisplay")) 
  {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1e3);
  }
  Serial.println("Connected! :)");
  wifiStatus = CONNECTED;
  pConfig->saveOnDemand(); // Will save if there is need

  Udp.begin(pConfig->network.port);

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

  delay(1e3); // Let wifi animation stay for a while
  display.setMode(MyDisplay::Mode::NORMAL);
  delay(1e2); // Wait for last animation plot before we clear
  display.clear(MyDisplay::Layer::ALL);

  display.test();
  display.disco();
  display.clear();

  /* Read EEPROM */
  pConfig->load();
  Serial.printf("Effect: %d\n", (MyDisplay::Effect)pConfig->display.effect);
  display.setEffect((MyDisplay::Effect)pConfig->display.effect);
  pConfig->print();

  display.setMode(MyDisplay::Mode::CLOCK);
  
  myClock.update();
  clockTicker.attach_ms(1e3, clockISR);
}

void loop()
{
  counter++;

  if (clockTaskReady)
  {
    clockTaskReady = false;
    myClock.tick();
    //Serial.printf("%d:%d:%d\n", clock.getTime().hour, clock.getTime().minute, clock.getTime().second);
  } 
  if (mainTaskReady)
  {
    mainTaskReady = false;
    mainWorker();
  }
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
