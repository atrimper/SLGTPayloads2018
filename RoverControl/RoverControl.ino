#include <SparkFunTSL2561.h>
#include <Wire.h>
#include "Motor.h"

SFE_TSL2561 light;
Motor right(0, 1, 2, false);
Motor left(3, 4, 5, true);

boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
double threshold = 100;

void setup()
{
  // Initialize the Serial port:
  
  Serial.begin(9600);
  light.begin();
  unsigned char ID;
  
  gain = 0;
  unsigned char time = 2;
  light.setTiming(gain,time,ms);
  light.setPowerUp();
}

void loop()
{
  double lux; // Resulting lux value
  delay(30000);
  while (lux < threshold)
  {
    delay(ms);
    unsigned int data0, data1;
    if (light.getData(data0,data1))
    { 
      light.getLux(gain,ms,data0,data1,lux);
    }
  }
  delay(2000);
  right.setSpeed(1);
  left.setSpeed(1);
  delay(5000);
  right.setSpeed(0);
  left.setSpeed(0);
}
