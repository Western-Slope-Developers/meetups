// Detect switch closed (OOP Version)

#include <WWSunFounderUtils.h>

/***************** Global Objects ************************/
// Global objects are constructed BEFORE setup is called.

const int ledPin = 5;         // Port number LED is connected to.
const int pushButtonPin = 7;  // Port number pushbutton switch is connected to.

// Construct led object passing port pin and active state.
sfu::DigitalLed led( ledPin );

// Construct pushbutton object passing port pin, active state and activate internal pullup.
sfu::PushButton pushButton( pushButtonPin );

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
  if ( pushButton.closed() )
  {
    led.on();    
  }
  else
  {
    led.off(); 
  }
}
/************************************/

