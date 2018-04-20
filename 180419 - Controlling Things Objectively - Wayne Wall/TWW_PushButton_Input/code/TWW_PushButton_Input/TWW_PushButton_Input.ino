// Detect pushbutton closed
/**********************************/
const int pushButtonPin = 7;  // Port number pushbutton is connected to.
const int ledPin = 5;         // Port number led is connected to.
/**********************************/
// Called once at start up
void setup()
{
  pinMode(pushButtonPin, INPUT);  // Initialize the pushbutton pin as input port
  pinMode(ledPin, OUTPUT);        // Initialize the led pin as output port
}
/**********************************/
// Called repeatedly until the reset button is pressed
void loop()
{
  // If pushbutton is closed...
  if ( digitalRead(pushButtonPin) == LOW )
  {
    digitalWrite(ledPin, HIGH); // Turn on the led    
  }
  else
  {
    digitalWrite(ledPin, LOW);  // Turn off the led   
  }
}
/************************************/

