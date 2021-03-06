#include "Wifi.h"
#include "components/Utility/Log.h"

Wifi::Wifi()
{
}

void Wifi::setup()
{
  Log::Logger()->write(Log::Level::INFO, "Started Wifi Setup");
  config = Config::get();
  handleClientInterval = new Metro(250);

  //Wifi
  // Store address of the WiFiServeir
  this->server = new WiFiServer(config->wifi.port);
  const char *ssid = config->wifi.ssid.c_str();
  const char *password = config->wifi.password.c_str();

  IPAddress address(config->wifi.ip[0], config->wifi.ip[1], config->wifi.ip[2], config->wifi.ip[3]);
  IPAddress subnet (config->wifi.subnet[0], config->wifi.subnet[1], config->wifi.subnet[2], config->wifi.subnet[3]);
  delay(200); // give time to set the addess and subnet. Otherwise we lose the configs.
  //If we dont disable the wifi, then there will be some issues with conncting to the device sometimes
  WiFi.disconnect(true);
  byte channel = config->wifi.channel;
  float wifiOutputPower = 20.5; //Max power
  WiFi.setOutputPower(wifiOutputPower);
  WiFi.setPhyMode(WIFI_PHY_MODE_11B);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_AP);
  //C:\Users\spe\AppData\Roaming\Arduino15\packages\esp8266\hardware\esp8266\2.1.0\cores\esp8266\core_esp8266_phy.c
  //TRYING TO SET [114] = 3 in core_esp8266_phy.c 3 = init all rf
  WiFi.persistent(false);
  WiFi.softAPConfig(address, address, subnet);
  WiFi.softAP(ssid, password, channel);
  IPAddress myIP = WiFi.softAPIP();

  server->begin();
  //Set delay = true retarts the esp in version 2.1.0, check in later versions if its fixed
  server->setNoDelay(true);

  Log::Logger()->write(Log::Level::INFO, "Finished Wifi Setup");

}


/**
 * Returns true if device is requesting registration.
 * TODO:: Have it take the id and type and store it as a temporary pending device.
 */

bool Wifi::handleClientConnections() {
  //Serial.printf("Stations connected to soft-AP = %d\n", WiFi.softAPgetStationNum());
  yield();
  if (handleClientInterval->check() == 1)
  {
    if (server->hasClient())
    {
      client = server->available();
      if (client && client.connected())
      {
        client.setNoDelay(true);
        // TODO: Once the mobile app is ready, get the id and name from there.
        Log::Logger()->write(Log::Level::DEBUG, "New pending wifi object received.");

        pendingDevice.id[0] = 'W';
        pendingDevice.id[1] = 'I';
        pendingDevice.id[2] = 'F';
        pendingDevice.id[3] = 'I';
        pendingDevice.id[4] = '1';
        pendingDevice.type  =  1; // wifi
        pendingDevice.pending = true;

        return true;
      }
      else
      {
        client.stop();
      }
    }
    return false;
  }
  yield();
}

void Wifi::clearPendingDevice()
{
  Log::Logger()->write(Log::Level::DEBUG, "Clearing Wifi Pending Device");
  // Setting back to default
  for (byte i = 0; i < 5; i++) {
    pendingDevice.id[i] = 0;
  }
  pendingDevice.pending = false;
  pendingDevice.type = 0;
  //this->printDeviceCredentials(pendingDevice);
}
