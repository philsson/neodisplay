#include "config.h"

#include <EEPROM.h>

static bool needSaving = false;

void saveWiFiConfigCallback()
{
    needSaving = true;
}

Settings *Settings::s_instance = 0;

Settings::Settings()
: m_wifiManager()
, m_addrNetwork(0)
, m_addrDisplay(m_addrNetwork + sizeof(Settings::network))
, m_wmPort("Port", "Incoming UDP port", wifiManager.port, WM_PORT_NUM_SIZE)
, m_wmNumOfLEDs("NumOfLeds", "NumOfLeds", wifiManager.numOfLEDs, 4)
, m_wmDefaultEffect("DefaultEffect", "DefaultEffect", wifiManager.defaultEffect, 4)
{
    m_wifiManager.setSaveConfigCallback(saveWiFiConfigCallback);
    m_wifiManager.addParameter(&m_wmPort);
    m_wifiManager.addParameter(&m_wmNumOfLEDs);
    m_wifiManager.addParameter(&m_wmDefaultEffect);
}

Settings* Settings::Instance()
{
    if (!s_instance)
    {
        s_instance = new Settings;
    }
    return s_instance;
}

WiFiManager* Settings::getWiFiManager()
{
    return &m_wifiManager;
}

bool Settings::load()
{
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(m_addrNetwork, network);
    EEPROM.get(m_addrDisplay, display);
    EEPROM.end();
    return true;
}

bool Settings::save()
{
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(m_addrNetwork, network);
    Serial.printf("Saving conf with Mode %d\n", display.effect);
    EEPROM.put(m_addrDisplay, display);
    EEPROM.commit();
    EEPROM.end();
    return true;
}

bool Settings::saveOnDemand()
{
    if (needSaving)
    {
        network.port = atoi(m_wmPort.getValue());
        return save();
    }
    return true;
}

void Settings::reconfigure()
{
    if (!m_wifiManager.startConfigPortal("NeoDisplay"))
    {
        delay(1000);
        ESP.reset();
    }
}

void Settings::resetWiFi()
{
    m_wifiManager.resetSettings();
    ESP.reset();
}

void Settings::print()
{
    Serial.printf("\n#-------- Config --------#\n");
    Serial.printf("# UDP Port: %d\n", network.port);
    Serial.printf("# Number of LEDs: %d\n", display.numOfLeds);
    Serial.printf("# Display effect: %d\n", display.effect);
}

/*  Documentation for ESP8266 EEPROM

    // commit between 4 and 4096 bytes of ESP8266 flash (for "EEPROM" emulation)
    // this step actually loads the content (e.g. 512 bytes) of flash into 
    // a 512-byte-array cache in RAM
    EEPROM.begin(512);

    // read bytes (i.e. sizeof(data) from "EEPROM"),
    // in reality, reads from byte-array cache
    // cast bytes into structure called data
    EEPROM.get(addr,data);

    // replace values in byte-array cache with modified data
    // no changes made to flash, all in local byte-array cache
    EEPROM.put(addr,data);

    // actually write the content of byte-array cache to
    // hardware flash.  flash write occurs if and only if one or more byte
    // in byte-array cache has been changed, but if so, ALL 512 bytes are 
    // written to flash
    EEPROM.commit();  
*/