# neodisplay
NodeMCU based neopixel display. I am using a 8x36, 256 pixel display when developing this project. The display will eventually show time, do animations, games etc.

## NodeMCU
The display is driven by a NodeMCU connected to your WiFi. Primarelly the display will receive updates through UDP. By itself it will have some features as for example showing the time.

### WiFi and Initial Configuration
For WiFi configuration we make use of the library WiFiManager by tzapu. If never configured, the board will start its own access point called NeoDisplay. Browse its default IP address @192.168.4.1 to setup. Other settings may be changes such as the receiving UDP port etc.
Once setup these changes are stored in its own sector in memory to be permanently stored. At the moment there is no way to "flush" these settings other than changing the sketch calling a reset. A button command on the built in button is set up among the "issues" for this project.

## Python
It is here the display really comes to life. Here we implement the big features. My initial plan is to implement rolling text, snake and Tetris.
