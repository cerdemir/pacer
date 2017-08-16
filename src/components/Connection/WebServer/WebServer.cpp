#include "WebServer.h"
#include <ESP8266WebServer.h>
WebServer::WebServer()
{

}

void WebServer::setup()
{
  Serial.println("Setting up webServer");
  this->webServer = new ESP8266WebServer(80);
  httpUpdater.setup(this->webServer, update_path, update_username, update_password);
  this->webServer->begin();
  Serial.println("Finished setting up webServer");
}


void WebServer::handleClient()
{
  this->webServer->handleClient();
}
