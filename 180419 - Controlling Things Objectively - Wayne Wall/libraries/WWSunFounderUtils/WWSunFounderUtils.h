// Utility classes header file for SunFounder Arduino sensors/devices

#ifndef INCLUDE_WW_SUNFOUNDER_UTILS
#define INCLUDE_WW_SUNFOUNDER_UTILS

// #define WW_SUNFOUNDER_UTILS_ARDUINO if compiling for the Arduino.
// #define WW_SUNFOUNDER_UTILS_RPI if compiling for the Raspberry Pi.

#define WW_SUNFOUNDER_UTILS_ARDUINO
// #define WW_SUNFOUNDER_UTILS_RPI

#ifdef WW_SUNFOUNDER_UTILS_ARDUINO
#include <Arduino.h>            // Basic Arduino declarations
#endif

#ifdef WW_SUNFOUNDER_UTILS_RPI
#include <wiringPi.h>           // Basic RPi wiring library
#endif

#include <LiquidCrystal_I2C.h>  // Declarations for SunFounder LCD using I2C data transfer
#include <dht.h>                // Declarations Humiditure sensor

namespace sfu
{

// Passed to various constructors to activate/deactivate an input port's internal pull up resistor.
static const bool InternalPullUp = true;
static const bool NoInternalPullUp = false;

// Class whose ctor performs any one-time initialization needed by the library.

// *** IMPORTANT *** The gcc compiler requires the UtilityInit ctor take an argument,
// otherwise, the ctor will NOT be called at startup before the program's "main" entry
// point is called.  Crazy stuff!!

class UtilityInit
{
   public:
   UtilityInit(int dummy);
};

/********************** DIGITAL OUTPUT *****************************/
// A DigitalOutput is associated with a HIGH/LOW output port and can
// only be in one of two states: "active" or "inactive".

class DigitalOutput
{
  protected:
  // activeState = HIGH means device is active if port is HIGH
  // activeState = LOW  means device is active if port is LOW  
  DigitalOutput(int pinNumber, int activeState);
  
  public:
  void active();      // Set output port to active state
  void inactive();    // Set output port to inactive state
  
  // Returns which state (HIGH or LOW) is considered the active state
  int getActiveState() const;
  
  private:
  int m_pinNumber;    // Output port pin number
  int m_activeState;  // Port state (HIGH/LOW) which activates device
};

/********************** DIGITAL INPUT ******************************/
// A DigitalInput is associated with an input port and can only be
// active or inactive.

class DigitalInput
{
  protected:
  // activeState = HIGH means input is active if port is HIGH
  // activeState = LOW  means input is active if port is LOW
  // activateInternalPullUp = "true" if internal pullup resistor is to be activated
  DigitalInput(int pinNumber, int activeState, bool activateInternalPullUp);  
  
  public:
  bool active() const;    // Returns "true" if active, "false" if inactive
  bool inactive() const;  // Returns "true" if inactive, "false" if active
  
  // Returns which state (HIGH or LOW) is considered the active state
  int getActiveState() const;
  
  private:
  int m_pinNumber;    // Input port pin number
  int m_activeState;  // Port state (HIGH/LOW) which means input is active
};

/*********************** ANALOG OUTPUT *****************************/
// An AnalogOutput is associated with an output port that can take on
// integer values from minOutput to maxOutput.  minOutput will produce
// the lowest voltage output, maxOutput the highest.

class AnalogOutput
{
  protected:
  AnalogOutput(int pinNumber, int minOutput = 0, int maxOutput = 255);
  
  public:
  // Output a fraction of the voltage range
  // 0.0 = minimum voltage
  // 1.0 = maximum voltage
  void output(double fraction);
  
  // Output a port voltage that corresponds to the value passed
  void outputPortValue(int value);
  
  int getMinOutputValue() const;  // Returns value which produces minimum output voltage
  int getMaxOutputValue() const;  // Returns value which produces maximum output voltage
  
  private:
  int m_pinNumber;  // Output port pin number
  int m_minOutput;  // Value which produces minimum output voltage
  int m_maxOutput;  // Value which produces maximum output voltage
};

/*********************** ANALOG INPUT ******************************/
// An AnalogInput is associated with an input port and can take on
// integer values from minInput to maxInput.  minInput represents
// the lowest voltage possible, maxInput the highest.

class AnalogInput
{
  protected:
  // By default, SunFounder analog inputs return values ranging from 0 to 1023
  AnalogInput(int pinNumber, int minInput = 0, int maxInput = 1023);
  
  public:
  // Get the fraction of the possible input voltage range.
  // 0.0 = minimum voltage
  // 1.0 = maximum voltage
  double input() const;
  
  // Get the value corresponding to the input port voltage.
  int inputPortValue() const;
  
  int getMinInputValue() const;   // Returns value corresponding to minimum input voltage
  int getMaxInputValue() const;   // Returns value corresponding to maximum input voltage
  
  private:
  int m_pinNumber;  // Input port pin number
  int m_minInput;   // Value which produces minimum input voltage
  int m_maxInput;   // Value which produces maximum input voltage
};

/************************ DIGITAL RELAY ******************************/
// A DigtialRelay is associated with a HIGH/LOW output port and is either
// closed (active) or open (inactive).

class DigitalRelay : public DigitalOutput
{
  public:
  DigitalRelay(int pinNumber, int activeState = LOW)
  : DigitalOutput(pinNumber, activeState) { }
  
  void open()   { inactive(); }
  void close()  { active(); }
};

/************************ DIGITAL HEIGHT CONTROL ******************************/
// A DigtialHeightControl can be enabled or disabled via an output port.
// It can make an external device go "up" or "down" via two separate HIGH/LOW output ports.

class DigitalHeightControl
{
  public:
  DigitalHeightControl(int controlPin, int upPin, int downPin, int activeState = LOW);
  
  void enable()  { m_enableControl.close(); idle(); }
  void disable() { m_enableControl.open(); idle(); }
  void up();
  void down();
  void idle();
  
  private:
  DigitalRelay  m_enableControl;  // Set LOW to enable height control.
  DigitalRelay  m_up;             // Set LOW to raise device.
  DigitalRelay  m_down;           // Set LOW to lower device;
};

/************************ DIGITAL LED ******************************/
// A DigtialLed is associated with a HIGH/LOW output port and is either
// on (active) or off (inactive).

class DigitalLed : public DigitalOutput
{
  public:
  DigitalLed(int pinNumber, int onState = HIGH)
  : DigitalOutput(pinNumber, onState) { }
  
  void on()   { active(); }
  void off()  { inactive(); }
};

/************************* ANALOG LED ******************************/
// An AnalogLed is associated with an analog (or pulse width modulation)
// output port.  minOutput represents minimum intensity, maxOutput
// represents maximum intensity.

class AnalogLed : public AnalogOutput
{
  public:
  AnalogLed(int pinNumber, int minOutput = 0, int maxOutput = 255)
  : AnalogOutput(pinNumber, minOutput, maxOutput) { }

  void intensity(double fraction) { AnalogOutput::output(fraction); }
};

/*********************** RED GREEN LED *****************************/
// A RedGreenLed internally has a red analog LED connected to one output
// port pin and a green analog LED connected to a different analog output
// port pin.  The intensity of each LED can be controlled by setting a value
// between minOutput and maxOutput;

class RedGreenLed
{
  public:
  RedGreenLed(int redPin, int greenPin, int minOutput = 0, int maxOutput = 255);

  // Set LED to various common colors
  void red();
  void green();
  void orange();
  
  void on();    // Turn LED full "on" (max intensity both red and green)
  void off();   // Turn LED full "off" (min intensity both red and green)

  // Set the fraction of red and green intensities
  void intensity(double redFraction, double greenFraction);
  
  // Set the values of red and green intensities
  void intensity(int redValue, int greenValue);
  
  private:
  AnalogLed m_redLed;
  AnalogLed m_greenLed;
};

/************************** RGB LED ********************************/
// An RGBLed internally has a red analog LED, a green analog LED and
// a blue analog LED connected to separate output port pins. The intensity
// of each LED can be controlled by setting a value between minOutput
// and maxOutput.

class RGBLed
{
  public:
  // SunFounder RGB LED output values are minOutput = 255 and maxCoutput = 0
  RGBLed(int redPin, int greenPin, int bluePin, int minOutput = 255, int maxOutput = 0);

  // Set LED to various common colors
  void red();
  void orange();
  void yellow();
  void green();
  void blue();
  void indigo();
  void purple();
  void white();
  
  void on();    // Turn LED full "on" (max intensity red, green and blue)
  void off();   // Turn LED full "off" (min intensity red, green and blue)
  
  // Set the fraction of the red, green and blue intensities
  void intensity(double redFraction, double greenFraction, double blueFraction);
  
  // Set the values of the red, green and blue intensities
  void intensity(int redValue, int greenValue, int blueValue);
  
  private:
  AnalogLed m_redLed;
  AnalogLed m_greenLed;
  AnalogLed m_blueLed;
};

/*********************** ANALOG MOTOR ******************************/
// An AnalogMotor is associated with an analog (or pulse width modulation)
// output port.  minOutput represents minimum motor power, maxOutput
// represents maximum motor power.

class AnalogMotor : public AnalogOutput
{
  public:
  AnalogMotor(int pinNumber, int minOutput = 0, int maxOutput = 255)
  : AnalogOutput(pinNumber, minOutput, maxOutput) { }

  void power(double fraction) { AnalogOutput::output(fraction); }
};

/*********************** ACTIVE BUZZER *****************************/
// An ActiveBuzzer is associated with a HIGH/LOW output port and is
// either on (active) or off (inactive).

class ActiveBuzzer : public DigitalOutput
{
  public:
  ActiveBuzzer(int pinNumber, int onState = LOW);
  
  void on()     { active(); }
  void off()    { inactive(); }
  
  void alert(); // Generate one special "alert" beep cycle.
  
  void beep(int durationMilliSeconds);
};

/********************** PASSIVE BUZZER *****************************/
// A PassiveBuzzer is associated with a HIGH/LOW output port, but its
// tone must be generated in software.  The tone frequency (in cycles/sec)
// is specified via the "setTone" method.  The "beep" method
// generates the tone for the specified number of milliseconds.

class PassiveBuzzer
{
  public:
  PassiveBuzzer(int pinNumber);
  
  void tone(int cyclesPerSecond, int durationMilliSeconds);
  
  private:
  int m_pinNumber;      // Pin number of associated output port.
};

/************************ PWM BUZZER *******************************/
// A PWMBuzzer is associated with a HIGH/LOW Pulse Width Modulation
// output port and generates fixed frequency (50% HIGH, 50% LOW)
// tone using the port's default PWM generator.

class PWMBuzzer : public AnalogOutput
{
  public:
  PWMBuzzer(int pinNumber);
  
  void on();      // Start generating the tone.
  void off();     // Stop generating the tone.
  void alert();   // Generate one special "alert" beep cycle.

  void beep(int durationMilliSeconds);  // Generate tone for specified duration.
};

/********************** DIGITAL SWITCH ******************************/
// A DigitalSwitch is associated with a digital input port and can be
// either open (inactive) or closed (active).

class DigitalSwitch : public DigitalInput
{
  public:  
  DigitalSwitch(int pinNumber, int closedState = LOW, bool activateInternalPullUp = NoInternalPullUp)
  : DigitalInput(pinNumber, closedState, activateInternalPullUp) { }
  
  bool  open() const    { return inactive(); }
  bool  closed() const  { return active(); }
};

/************************ PUSH BUTTON *******************************/
// A PushButton is a DigitalSwitch which is momentarily closed when pushed.
// It's either open (inactive) or closed (active).

class PushButton : public DigitalSwitch
{
  public:  
  PushButton(int pinNumber, int closedState = LOW, bool activateInternalPullUp = NoInternalPullUp)
  : DigitalSwitch(pinNumber, closedState, activateInternalPullUp) { }
};

/*********************** TILT SWITCH *******************************/
// A TiltSwitch is a DigitalSwitch whose closure is activated by
// a metal ball inside.  It's either open (inactive) or closed (active).

class TiltSwitch : public DigitalSwitch
{
  public:  
  TiltSwitch(int pinNumber, int closedState = LOW, bool activateInternalPullUp = NoInternalPullUp)
  : DigitalSwitch(pinNumber, closedState, activateInternalPullUp) { }
};

/******************** HALL EFFECT SWITCH ***************************/
// A HallEffectSwitch is a DigitalSwitch whose closure is activated by
// a magnet in proximity.  It's either open (inactive) or closed (active).

class HallEffectSwitch : public DigitalSwitch
{
  public:  
  HallEffectSwitch(int pinNumber, int closedState = LOW, bool activateInternalPullUp = NoInternalPullUp)
  : DigitalSwitch(pinNumber, closedState, activateInternalPullUp) { }
};

/****************** PHOTO INTERRUPT SWITCH *************************/
// A PhotoInterruptSwitch is a DigitalSwitch whose closure is activated by
// a light beam being interrupted.  The switch is "open" when the light
// beam is NOT interrupted.

class PhotoInterruptSwitch : public DigitalSwitch
{
  public:  
  PhotoInterruptSwitch(int pinNumber, int closedState = HIGH, bool activateInternalPullUp = NoInternalPullUp)
  : DigitalSwitch(pinNumber, closedState, activateInternalPullUp) { }
};

/******************** IR PROXIMITY SWITCH **************************/
// A IRProximitySwitch is a DigitalInput that's activated when the sensor
// is in proximity of an obstacle ahead (i.e., within a preset distance
// from an obstacle in front of the sensor). It's considered "closed"
// if it's in proximity and "open" if not.

class IRProximitySwitch : public DigitalSwitch
{
  public:
  IRProximitySwitch(int pinNumber, int onState = LOW, bool activateInternalPullUp = NoInternalPullUp)
  : DigitalSwitch(pinNumber, onState, activateInternalPullUp) { }
};

/********************** TRACKING SENSOR ****************************/
// A TrackingSensor is a DigitalInput that's activated when the sensor
// passes over a black line (that's at least as wide as the sensor).
// It's either "on" or "off" the line.

class TrackingSensor : public DigitalSwitch
{
  public:
  TrackingSensor(int pinNumber, int onState = HIGH, bool activateInternalPullUp = NoInternalPullUp)
  : DigitalSwitch(pinNumber, onState, activateInternalPullUp) { }
  
  bool on() const   { return active(); }
  bool off() const  { return inactive(); }
};


/******************** ANALOG INPUT VOLTAGE *************************/
// An AnalogInputVoltage object represents a voltage present at
// an analog input port pin.

class AnalogInputVoltage : public AnalogInput
{
  public:
  // Pass ctor input port pin number, min & max input values, then the
  // voltages that correspond to those input values.
  AnalogInputVoltage(int pinNumber, int minValue = 0, int maxValue = 1023,
                     double minVoltage = 0.0, double maxVoltage = 5.0);

  // Returns the voltage currently at the analog input port
  double voltage() const;
  
  // Returns the current fraction of the voltage range.
  // (0.0 = min voltage, 1.0 = max voltage)
  double voltageFraction() const { return input(); }

  private:
  double  m_minVoltage;   // Min voltage value
  double  m_maxVoltage;   // Max voltage value
};

/********************** JOY STICK AXIS *****************************/
// A JoyStickAxis is associated with an analog input port.  Input values
// range from some value representing the minimum input to some value
// representing the maximum input.

class JoyStickAxis : public AnalogInput
{
  public:
  // Each SunFounder joystick axis returns values ranging from 0 to 1023
  JoyStickAxis(int pinNumber, int minInput = 0, int maxInput = 1023)
  : AnalogInput(pinNumber, minInput, maxInput) { }

  // Return input voltage as fraction (0.0 to 1.0) of input range
  double inputFraction() const { return AnalogInput::input(); }
  
  // Return value corresponding to input voltage.
  int inputValue() const { return AnalogInput::inputPortValue(); }
};

/************************ JOY STICK ********************************/
// A JoyStick has a X-axis analog input and a Y-axis analog input,
// each connected to a separate analog input port.  Each input can
// produce values in the range minInput to maxInput.

class JoyStick
{
  public:
  // Each SunFounder joystick axis returns values ranging from 0 to 1023
  JoyStick(int analogXPin, int analogYPin, int minInput = 0, int maxInput = 1023)
  : m_xAxis(analogXPin, minInput, maxInput),
    m_yAxis(analogYPin, minInput, maxInput) { }

  // Return fraction of x, y input range
  double xFraction() const  { return m_xAxis.inputFraction(); }
  double yFraction() const  { return m_yAxis.inputFraction(); }
  
  // Return x, y input value
  int xValue() const  { return m_xAxis.inputValue(); }
  int yValue() const  { return m_yAxis.inputValue(); }
  
  private:
  JoyStickAxis  m_xAxis;
  JoyStickAxis  m_yAxis;
};

/********************* PHOTO RESISTOR ******************************/
// A PhotoResistor is an analog input whose value is inversely related
// to the amount of light striking the photo sensor.  The less light,
// the greater the value.

class PhotoResistor : public AnalogInput
{
  public:
  PhotoResistor(int analogPin, int minInput = 0, int maxInput = 1023)
  : AnalogInput(analogPin, minInput, maxInput) { }
};

/********************** SOUND SENSOR *******************************/
// A SoundSensor is an analog input whose value is directly related to
// the sound intensity entering a capacitive electret microphone.
// The greater the sound intensity, the greater the value.

class SoundSensor : public AnalogInput
{
  public:
  SoundSensor(int analogPin, int minInput = 0, int maxInput = 1023)
  : AnalogInput(analogPin, minInput, maxInput) { }
};

/********************** FLAME SENSOR *******************************/
// A FlameSensor is an analog input whose value is inversely related to
// infrared intensity detected.  The greater the intensity, the less the value.

class FlameSensor : public AnalogInput
{
  public:
  FlameSensor(int analogPin, int minInput = 0, int maxInput = 1023)
  : AnalogInput(analogPin, minInput, maxInput) { }
};

/********************** FLAME SWITCH *******************************/
// A FlameSwitch is a digital detector with a built-in threshold.  If
// the infrared intensity detected exceeds the threshold, the switch is
// considered "closed".  Otherwise, the switch is considered "open".

class FlameSwitch : public DigitalSwitch
{
  public:  
  FlameSwitch(int pinNumber, int closedState = LOW, bool activateInternalPullUp = NoInternalPullUp)
  : DigitalSwitch(pinNumber, closedState, activateInternalPullUp) { }
};

/******************* HALL EFFECT SENSOR ****************************/
// A HallEffectSensor is an analog input whose value is directly related
// to the proximity of a magnet.  The closer the magnet, the more the value.

class HallEffectSensor : public AnalogInput
{
  public:
  HallEffectSensor(int analogPin, int minInput = 0, int maxInput = 1023)
  : AnalogInput(analogPin, minInput, maxInput) { }
};

/**************** HALL EFFECT SENSOR SWITCH ************************/
// A HallEffectSensorSwitch is a digital detector with a built-in threshold.
// If the magnetic field intensity detected exceeds the threshold, the switch
// is considered "closed".  Otherwise, the switch is considered "open".

class HallEffectSensorSwitch : public DigitalSwitch
{
  public:  
  HallEffectSensorSwitch(int pinNumber, int closedState = LOW, bool activateInternalPullUp = NoInternalPullUp)
  : DigitalSwitch(pinNumber, closedState, activateInternalPullUp) { }
};

/******************* TOUCH SENSOR SWITCH ***************************/
// A TouchSensorSwitch is a digital detector.  Touch it and it flips
// state (from HIGH to LOW or LOW to HIGH).  The "closedState" ctor
// parameter determines which state (HIGH or LOW) is considered the
// switch "open" or "closed" state.

class TouchSensorSwitch : public DigitalSwitch
{
  public:  
  TouchSensorSwitch(int pinNumber, int closedState = LOW, bool activateInternalPullUp = NoInternalPullUp)
  : DigitalSwitch(pinNumber, closedState, activateInternalPullUp) { }
};

/******************** THRESHHOLD SWITCH ****************************/
// A ThreshHoldSwitch is an analog input with a threshhold value specified.
// The input value relative to the threshhold value determines whether the
// switch is considered "open" or "closed".

class ThreshHoldSwitch
{
  public:
  // analogInput is the associated analog input object.
  // threshHold is the threshhold value in the same units used by analogInput.
  //
  // closedState is HIGH if the switch is considered closed when the analog
  // input is greater than or equal to the threshhold value.
  //
  // closedState is LOW if the switch is considered closed when the analog
  // input is less than the threshhold value.
  ThreshHoldSwitch(const AnalogInput& analogInput, int threshHold, int closedState = HIGH);
  
  // Same as previous ctor, except threshHoldFraction is a fraction of the input value range.
  // (0.0 = min value, 1.0 = max value)
  ThreshHoldSwitch(const AnalogInput& analogInput, double threshHoldFraction = 0.5, int closedState = HIGH);
  
  bool open() const;
  bool closed() const; 

  int getClosedState() const  { return m_closedState; }
  int getThreshHold() const   { return m_threshHold; }
  
  private:
  const AnalogInput&  m_analogInput;
  int                 m_threshHold;
  int                 m_closedState;
};

/********************* HUMITURE SENSOR *****************************/
// A HumitureSensor is a humidity and temperature sensor in the same
// package.  The data must first be explicitly "sampled", then the last
// temperature (in degrees Celcius) or humidity (as a 0.0 to 100.0
// percentage) can be retrieved.

class HumitureSensor
{
  public:
  HumitureSensor(int pinNumber);
  
  // Sample (i.e., update) the humiture sensor data.
  // Returns "false" if an error occurred.
  bool sample();
  
  // Retrieve most recently sampled humiture data.
  double humidity();      // Humidity (as a 0.0 to 100.0 percentage).
  double temperature();   // Temperature (in degrees Celsius).
  
  private:
  int m_pinNumber;    // Input port pin sensor is connected to.
  dht m_dht;          // Object holding humiture sensor data.
};

/************************ LCD2LINE *********************************/
// An LCD2Line is a small LCD with two lines of 16 characters per line.
// It displays what's written to it until something else is written.

class LCD2Line
{
  public:
  LCD2Line();

  // One time setup.  Call from main Arduino "setup" routine.
  void setup();
  
  void displayTopLine(String lineText);     // Display top line of text
  void displayBottomLine(String lineText);  // Display bottom line of text
  
  // Display a line of text
  // lineNumber = display line (0 = top line, 1 = bottom line)
  // lineText = 16 characters to display on the line
  void display(int lineNumber, String lineText);
  
  private:
  LiquidCrystal_I2C  m_lcd; // SunFounder LCD using I2C data transfer
};

/********************* SERIAL MONITOR ******************************/
// A SerialMonitor uses the serial communications line between the
// Arduino and the IDE to send text and numbers.  The IDE will display
// output from the serial monitor in a separate window.

class SerialMonitor
{
  public:
  // Pass ctor the serial data rate (in bits per second)
  SerialMonitor(int dataRate = 9600);

  // Call from Arduino's main "setup" routine
  void setup();
  
  // Send text representation of values of various types (does NOT advance to a new line)
  void sendIntegerValue(int value);
  void sendBooleanValue(bool value);
  void sendDoubleValue(double value, int decimals = 3); // decimals = number of decimal places
  void sendDigitalLogicValue(int value);  // Pass in the value representing a HIGH or a LOW
  
  // Send some text (does NOT advance to a new line)
  void sendText(String text);
  
  // Send a "newline" sequence to advance to the beginning of a new line
  void sendNewline();
  
  private:
  int m_dataRate;   // Holds serial data rate (in bits per second)
};

}   // End "namespace sfu"

#endif    // INCLUDE_WW_SUNFOUNDER_UTILS
