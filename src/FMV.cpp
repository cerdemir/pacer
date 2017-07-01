#include "FMV.h"
#include "components/Connection/RadioDevice.h"

FMV::FMV()
{
}

void FMV::setup() {
  this->configController = new ConfigController;
  this->configController->loadConfig();
  this->connectionManager = new ConnectionManager(configController);
  this->connectionManager->setup();
  this->controllerManager = new ControllerManager(configController, connectionManager);

  RadioDevice accelController;
  accelController.id[0] = 'A';
  accelController.id[1] = 'C';
  accelController.id[2] = 'L';
  accelController.id[3] = 'C';
  accelController.id[4] = '1';
  accelController.type  = 3;
  this->controllerManager->registerController(accelController);
}

void FMV::loop()
{
  // Step 1. Check for a physical device that is trying to connect
//  if (stopScanning == false)
//  {
  this->connectionManager->handleClientConnections(); // detects new device and sets it as pending.
//  }

  // Step 2. Register physical devices as a controller
  this->handlePendingConnectionDevices(); // try to register the pending controllers if any are waiting.

  // Step 3. Set active a controller. // NOTE:: now we automatically enable the active controller.
  if(this->controllerManager->activeController != nullptr)
  {
    this->controllerManager->activeController->enable();
  }

  // // Step 4. Handle the Active Controller.
  byte status = this->controllerManager->handleController();
  if (status == 2)
  {
    Serial.println("FMV detected lost connection on the active controller.");
    // We know that the connection was lost. What now?
    this->controllerManager->unsetActiveController(); // This makes sure that we don't ask the radio to change to the active Device anymore.
    // TODO: Have a metro timer. We want to give the original active controller a chance to reconnect.
    //this->controllerManager->tryOtherControllers();
  }
  // this->controllerManager->printRegisteredControllers();
  // this->controllerManager->printActiveController();

}


/**
  * Gets the pending registration devices from all connection interfaces and attempts to register them as controllers.
*/
void FMV::handlePendingConnectionDevices()
{
  if (this->connectionManager->pendingDevice.pending == true)
  {
   bool registered = this->controllerManager->registerController(this->connectionManager->pendingDevice);
   if (!registered)
   {
     // We need an abstrac connection class here with virtual reset functions
     //this->connectionManager->radio->initPackets(); // nullify the packets.
     this->connectionManager->clearPendingDevice(); // zerofiy the pendingDevice.
     stopScanning = true; // flag for stoping the scanning of new devies.
   }
  }
}
