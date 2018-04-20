// Motor speed control

#include <WWSunFounderUtils.h>

const int motorControlPin = 9;  // Output pin controlling motor power.
const int statusLedPin = 5;     // Output pin status led is connected to.

// Construct global objects.

sfu::AnalogMotor  motorControl(motorControlPin);
sfu::AnalogLed    statusLed(statusLedPin);

/**********************************/
// Called once at start up
void setup()
{
  // Nothing more to do here.       
}

/**********************************/
// Called repeatedly until the reset button is pressed
void loop()
{
  MotorPower(1.0, 10000);
  MotorPower(0.75, 10000);
  MotorPower(0.5, 10000);
  MotorPower(0.25, 10000);
  MotorPower(0.0, 10000);
}

// Apply power to the motor. (0.0 = no power, 1.0 = max power)

void MotorPower(double power, int timeInMilliseconds)
{
  motorControl.power(power);
  statusLed.intensity(power);
  delay(timeInMilliseconds); 
}



