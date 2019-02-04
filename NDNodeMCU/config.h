/*
*  General configuration file
*  
*/
#pragma once

#include <Arduino.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager


/* WiFi / Network settings */
// Default incoming UDP port 4210

/* Display 
 * 0,0 is in the upper left corner
 * Width is on the x-axis
 * Height is on the y-axis
 */
#define NUM_PIXELS 256
#define PIN_PIXELS 14
#define UPPER_LEFT 0
#define LOWER_LEFT 7
#define UPPER_RIGHT 255
#define LOWER_RIGHT 248
#define WIDTH 32
#define HEIGHT 8
#define MAX_BRIGHTNESS 150

/* Timers */
#define LOOPTIME 20 // ms

/* Clock */
#define TIMEZONE 1

/* LEDs */
#define LED_MAIN D0

/* EEPROM */
#define EEPROM_SIZE 512

/* WIFI MANAGER */
#define WM_PORT_NUM_SIZE 8
#define WM_LED_NUM_SIZE 4

void saveWiFiConfigCallback();

class Settings 
{
public:

    struct {
        uint port = 4210;

    } network;

    struct {
        uint16_t numOfLeds = 4;
        uint8_t mode = 0;
    } display;

    // This struct is for temporary data
    // used by the wifiManager
    struct {
        char port[WM_PORT_NUM_SIZE] = "4210";
        char numOfLEDs[WM_LED_NUM_SIZE] = "4";
    } wifiManager;

    Settings(WiFiManager* pWiFiManager);

    //! Load config from EEPROM
    //! Return: Always true
    bool load();

    //! Save config to EEPROM
    //! Return: Always true
    bool save();

    //! Copy values from RAM and run
    //! save() to store in EEPROM
    bool saveOnDemand();

    //! Call to start AP mode and reconfigure
    //! If nothing is done settings will be preserved
    void reconfigure();

    //! Resets any stored values by the WiFiManager
    //! m_pWiFiManager
    void resetWiFi();

    void print();

private:

    WiFiManager* m_pWiFiManager;

    uint m_addrNetwork, m_addrDisplay;

    WiFiManagerParameter m_wmPort;

};

extern Settings config;
