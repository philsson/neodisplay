/*
*  General configuration file
*  
*/
#pragma once


/* WiFi / Network settings */
#define UDP_PORT 4210
#define WIFI_SSID "****"
#define WIFI_PASSWORD "****"

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
#define LOOPTIME 20

/* LEDs */
#define LED_MAIN D0
