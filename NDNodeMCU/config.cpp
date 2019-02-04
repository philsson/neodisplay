#include "config.h"

#include <EEPROM.h>

static bool needSaving = false;

void saveWiFiConfigCallback()
{
    needSaving = true;
}

Settings::Settings(WiFiManager* pWiFiManager)
: m_pWiFiManager(pWiFiManager)
, m_addrNetwork(0)
, m_addrDisplay(m_addrNetwork + sizeof(Settings::network))
, m_wmPort("Port", "Incoming UDP port", wifiManager.port, WM_PORT_NUM_SIZE)
{
    m_pWiFiManager->setSaveConfigCallback(saveWiFiConfigCallback);
    m_pWiFiManager->addParameter(&m_wmPort);
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
    if (!m_pWiFiManager->startConfigPortal("NeoDisplay"))
    {
        delay(1000);
        ESP.reset();
    }
}

void Settings::resetWiFi()
{
    m_pWiFiManager->resetSettings();
    ESP.reset();
}

void Settings::print()
{
    Serial.printf("UDP Port: %d\n", network.port);
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