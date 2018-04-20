// Flash an LED
/**********************************/
const int ledPin = 5;       // Port number LED is connected to.
/**********************************/
// Called once at start up
void setup()
{
  pinMode(ledPin, OUTPUT);  // Initialize the led port for output.
}
/**********************************/
// Called repeatedly until the reset button is pressed
void loop()
{
  digitalWrite(ledPin, HIGH); // Turn on the led
  delay(1000);                // Delay (in milliseconds)
  digitalWrite(ledPin, LOW);  // Turn off the led
  delay(5000);                // Delay (in milliseconds)
}
/************************************/

