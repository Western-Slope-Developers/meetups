// Motor speed control

#include <WWSunFounderUtils.h>

const int motorControlPin = 9;  // Output pin controlling motor power.
const int statusLedPin = 5;     // Output pin status led is connected to.
const int speedControlPin = A0; // Input pin speed control potentiometer is connected to.

// Construct global objects.

sfu::AnalogMotor          motorControl(motorControlPin);
sfu::AnalogLed            statusLed(statusLedPin);
sfu::AnalogInputVoltage   speedControlVoltage(speedControlPin);

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
  double powerFactor = speedControlVoltage.voltageFraction();
  MotorPower(powerFactor, 100);
}

// Apply power to the motor. (0.0 = no power, 1.0 = max power)

void MotorPower(double power, int timeInMilliseconds)
{
  motorControl.power(power);
  statusLed.intensity(power);
  delay(timeInMilliseconds); 
}



