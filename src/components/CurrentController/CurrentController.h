#ifndef CurrentController_h
#define CurrentController_h
#include "Arduino.h" // TODO:: optimize the inclusions of the Arduino.h . Here we need if for the byte type to be available.
#include "../Config/Config.h"

class CurrentController {
  Config* config;
public:
  // Current control
  // Set by the config file.
  float defaultCurrentNeutral;
  float defaultCurrentAccelerationMax;
  float defaultCurrentAccelerationMin;
  float defaultCurrentBrakeMax;
  float defaultCurrentBrakeMin;

  //Set by the config file.
  byte defaultInputNeutral;
  byte defaultInputMinBrake;
  byte defaultInputMaxBrake;
  byte defaultInputMinAcceleration;
  byte defaultInputMaxAcceleration;
  float defaultSmoothAlpha;


  CurrentController();
  void setup(); // Configures itself and the CurrentController.
  float getMotorAccelerationCurrent(float previousControllerInput);
  float getMotorBrakingCurrent(float previousControllerInput);
  float getNeutralCurrent();
};

#endif
