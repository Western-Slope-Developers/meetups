// Flash an LED (OOP Version)

#include <WWSunFounderUtils.h>

/***************** Global Objects ************************/
// Global objects are constructed BEFORE setup is called.

const int ledPin = 5;   // Port number LED is connected to.

// Construct object passing port pin.
sfu::DigitalLed led( ledPin );

/**********************************/
// Called once at start up
void setup()
{
  // Nothing more to do here.
}

/**********************************/
// Called repeatedly until the reset button is pressed.
void loop()
{
  led.on();
  delay(1000);    // Delay (in milliseconds)
  led.off();
  delay(5000);
}


