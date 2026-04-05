#include <Arduino.h>
#include <WIFI.h>
#include <Ticker.h>

// WiFi credentials 
const char ssid[]      = "ssid";
const char password[]  = "psk";
const char HOST_NAME[] = "CYD-SBB-CLOCK";

Ticker wifiReconnectTimer;

void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) 
  {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.printf("WiFi connected. IP: %s\n", WiFi.localIP().toString().c_str());
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.printf("WiFi disconnected, reason: %d\n", info.wifi_sta_disconnected.reason);

      // Try to reconnect to the Wi-Fi with a timer every 5 seconds
      // Be shure that automatic reconnect is disabled
      // WiFi.setAutoReconnect(false);
      wifiReconnectTimer.once(5, []() {
        Serial.println("Trying to reconnect WiFi...");
        WiFi.disconnect(false, false);
        WiFi.begin();
      });
      break;
  }
}


/**
 * Establish the WiFi connection with router 
 * and set a hostname for the ESP32
*/
void initWiFi()
{
  Serial.println("Connecting to WiFi");
  WiFi.setHostname(HOST_NAME);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);                // crucial for AsyncWebServer
  WiFi.setAutoReconnect(false);
  WiFi.persistent(false);
  WiFi.onEvent(onWiFiEvent);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
}


/**
 * Print nearby WiFi networks with SSID und RSSI 
 */
void printNearbyNetworks()
{
  int n = WiFi.scanNetworks();
  printf(R"(
Nearby WiFi networks:
--------------------
)");
  for (int i = 0; i < n; i++)
  {
    printf("%s\t%d\r\n", WiFi.SSID(i).c_str(), WiFi.RSSI(i));
  }
}


/**
 * Use a raw string literal to print a formatted
 * string of WiFi connection details
*/
void printConnectionDetails()
{
  Serial.printf(R"(
Connection Details
------------------
  SSID         %s
  Hostname     %s
  IP-Address   %s
  MAC-Address  %s
  RSSI         %d (received signal strength indicator)
  )", WiFi.SSID().c_str(),
      //WiFi.hostname().c_str(),  // ESP8266
      WiFi.getHostname(),    // ESP32 
      WiFi.localIP().toString().c_str(),
      WiFi.macAddress().c_str(),
      WiFi.RSSI());
}