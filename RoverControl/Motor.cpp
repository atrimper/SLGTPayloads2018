#include "Motor.h"

Motor::Motor(int INA, int INB, int PWM_PIN, bool invDir = false)
{
	this->PWM_PIN = PWM_PIN;
	this->INA = INA;
	this->INB = INB;
  this->invDir = invDir;
  this->dir = 0;
  
	pinMode(INA, OUTPUT);
	pinMode(INB, OUTPUT);
	pinMode(PWM_PIN, OUTPUT);
}

void Motor::setSpeed(double speed)
{
	double scaledSpeed = abs(speed) * 255;
   
	if(speed > 0)
	{
    if (invDir)
    {
		  digitalWrite(INA, LOW);
  		digitalWrite(INB, HIGH);
    }
    else
    {
      digitalWrite(INA, HIGH);
      digitalWrite(INB, LOW);
    }
  		analogWrite(PWM_PIN, scaledSpeed);
  		dir = 1;
  }
	else if(speed < 0)
	{
	  if (invDir)
    {
      digitalWrite(INA, HIGH);
      digitalWrite(INB, LOW);
    }
    else
    {
      digitalWrite(INA, LOW);
      digitalWrite(INB, HIGH);
    }
		analogWrite(PWM_PIN, scaledSpeed);
		dir = -1;
	}
	else
	{
		analogWrite(PWM_PIN, 0);
		dir = 0;
	}
}

int Motor::getDirection()
{
	return dir;
}
