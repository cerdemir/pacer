#include "PhoneController.h"
#include "components/Utility/Log.h"


// Construct the PhoneController and the AbstractController
PhoneController::PhoneController(Wifi* wifi, AbstractDevice device)
 : AbstractController(device)
{
  Log::Logger()->write(Log::Level::DEBUG, "Started Construction of PhoneController: ");
  this->wifi = wifi;
  this->phone = device;
  this->connectionLostTimer = new Metro(_LOST_CONNECTION);
  Log::Logger()->write(Log::Level::DEBUG, "Finished Construction of PhoneController: ");
}


/**
  * This method is called in the main loop. It deals with reading and writing data from and to the controller.
  */
bool PhoneController::handleController()
{
  this->read();

  if (this->connectionLostTimer->check() == 1)
  {
    Log::Logger()->write(Log::Level::WARNING, "PhoneController has lost connection to physical device");
    return false;
  }

  return true; // TODO:: Inmplement Lost Connection.
}

/**
  * Confirms with the physical controller that it is enbled and listened to.
  */

bool PhoneController::enable()
{
  //TODO:: Enable Two Way Communication with the phone.
  //TODO:: Get Confirmation of being enabled from the phone and return true.
  //TODO:: If no confirmation is comming in return false.
  return true;
}

bool PhoneController::disable()
{
  return true;
}

// TODO:: This reading of client should be handled by the wifi and not the phone.
// TODO:: see Radio.h and Nunchuck.h implementation.
//NOTE:: the wifi->client is representative of the physical device now.
void PhoneController::read()
{
   uint8_t i;
   // Check clients for data
   if (wifi->client && wifi->client.connected()) {
     unsigned int  timeout   = 3;
     unsigned long timestamp = millis();

     // While no data is coming do yield();
     while (wifi->client.available() == 0 && ((millis() - timestamp) <= timeout)) {
       yield();
     }

     unsigned int streamLength = wifi->client.available();
     if (streamLength > 0) {
       // size_t len
       byte packetLength = 11;
       byte packetCount  = 0;
       byte m[packetLength];

       unsigned int packages = int(float(streamLength / packetLength));

       for (uint8_t ii = 0; ii < (packages - 1) * packetLength; ii++)
       {
         wifi->client.read();
       }
       for (uint8_t ii = 0; ii < packetLength; ii++)
       {
         m[packetCount++] = wifi->client.read();
       }
       Serial.println();
       yield();
       // Reset the lost connection timer.
       this->connectionLostTimer->reset();
       if (PhoneController::validateChecksum(m, packetCount))
       {
           yield();

           Log::Logger()->write(Log::Level::DEBUG, "PhoneController Read Sample: " + (String)m[4]);
           processInput(m[4]);
       }
       else
       {
         //Checksum invalid
       }
     }
   } // endif serverClients are connected

} // end readFromWifiClient


byte PhoneController::getChecksum(byte* array, byte arraySize)
{
  long validateSum = 0;
  for (byte i = 0; i < arraySize; i++) {
    validateSum += array[i];
  }
  validateSum -= array[arraySize - 1];
  return validateSum % 256;
}

bool PhoneController::validateChecksum(byte* array, byte arraySize)
{
  return array[arraySize - 1] == getChecksum(array, arraySize);
}
