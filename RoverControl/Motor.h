#ifndef _MOTOR_
#define _MOTOR_

#include "Arduino.h"

class Motor
{
	private:
		int PWM_PIN;
		int INA;
		int INB;
		int invDir;
    int dir;
  public:
  	Motor(int INA, int INB, int PWM_PIN, bool invDir);
  	void setSpeed(double speed);
    void setInvDir();
  	int getDirection();
};

#endif //ifndef _MOTOR_
