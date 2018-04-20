// Utility classes implementation file for SunFounder Arduino sensors/devices

#include "WWSunFounderUtils.h"

namespace sfu
{

UtilityInit utilityInit(0);

UtilityInit::UtilityInit(int /* dummy */)
{
  // Arduino requires no special initialization, but Raspberry Pi does.
  
#ifdef WW_SUNFOUNDER_UTILS_RPI
   wiringPiSetup();
#endif
}

/********************** DIGITAL OUTPUT *****************************/

DigitalOutput::DigitalOutput(int pinNumber, int activeState)
  : m_pinNumber(pinNumber),
    m_activeState(activeState)
{
  pinMode(m_pinNumber, OUTPUT);   // Init as an output port
  inactive();                     // Init to be inactive
}
  
void DigitalOutput::active()
{
  digitalWrite(m_pinNumber, m_activeState);
}
  
void DigitalOutput::inactive()
{
  digitalWrite(m_pinNumber, (m_activeState == HIGH) ? LOW : HIGH);
}
  
int DigitalOutput::getActiveState() const
{
  return m_activeState;
}

/********************** DIGITAL INPUT ******************************/

DigitalInput::DigitalInput(int pinNumber, int activeState, bool activateInternalPullUp)
  : m_pinNumber(pinNumber),
    m_activeState(activeState)
{
#ifdef WW_SUNFOUNDER_UTILS_ARDUINO
  if ( activateInternalPullUp )
  {
    pinMode(m_pinNumber, INPUT_PULLUP);
  }
  else
  {
    pinMode(m_pinNumber, INPUT);
  }
#endif

#ifdef WW_SUNFOUNDER_UTILS_RPI
  pinMode(m_pinNumber, INPUT);
  
  if ( activateInternalPullUp )
  {
    pullUpDnControl(PUD_UP);
  }
  else
  {
    pullUpDnControl(PUD_OFF);
  }
#endif
}
  
bool DigitalInput::active() const
{
  return digitalRead(m_pinNumber) == m_activeState;
}

bool DigitalInput::inactive() const
{
  return !active();
}
  
int DigitalInput::getActiveState() const
{
  return m_activeState;
}

/*********************** ANALOG OUTPUT *****************************/

AnalogOutput::AnalogOutput(int pinNumber, int minOutput, int maxOutput)
  : m_pinNumber(pinNumber),
    m_minOutput(minOutput),
    m_maxOutput(maxOutput)
{
  pinMode(m_pinNumber, OUTPUT);   // Init as an output port
  output(0.0);                    // Init to minimum output
}
  
void AnalogOutput::output(double fraction)
{
  if ( fraction < 0.0 ) fraction = 0.0;
  if ( fraction > 1.0 ) fraction = 1.0;
    
  int outValue = m_minOutput;
    
  if ( m_minOutput <= m_maxOutput )
  {
    // Output value increases from m_minOutput to m_maxOutput
    outValue += (int) ((m_maxOutput - m_minOutput) * fraction);
  }
  else
  {
    // Output value decreases from m_minOutput to m_maxOutput
    outValue -= (int) ((m_minOutput - m_maxOutput) * fraction);
  }

  outputPortValue(outValue);
}
  
void AnalogOutput::outputPortValue(int value)
{
  analogWrite(m_pinNumber, value);
}

int AnalogOutput::getMaxOutputValue() const
{
  return m_maxOutput;
}

int AnalogOutput::getMinOutputValue() const
{
  return m_minOutput;
}

/*********************** ANALOG INPUT ******************************/

AnalogInput::AnalogInput(int pinNumber, int minInput, int maxInput)
  : m_pinNumber(pinNumber),
    m_minInput(minInput),
    m_maxInput(maxInput) { }
  
double AnalogInput::input() const
{
  int value = inputPortValue();

  if ( m_minInput <= m_maxInput )
  {
    // Input value increases from m_minInput to m_maxInput
    return (double) (value - m_minInput) / (double) (m_maxInput - m_minInput);
  }
  else
  {
    // Input value decreases from m_minInput to m_maxInput
    return (double) (m_minInput - value) / (double) (m_minInput - m_maxInput);
  }  
}

int AnalogInput::inputPortValue() const
{
  return analogRead(m_pinNumber);
}

int AnalogInput::getMaxInputValue() const
{
  return m_maxInput;
}

int AnalogInput::getMinInputValue() const
{
  return m_minInput;
}


/******************* DIGITAL HEIGHT CONTROL ************************/

DigitalHeightControl::DigitalHeightControl(int controlPin, int upPin, int downPin, int activeState)
  : m_enableControl(controlPin, activeState),
    m_up(upPin, activeState),
	  m_down(downPin, activeState)
{
  disable();
}

void DigitalHeightControl::up()
{
	m_down.open();
	m_up.close();
}

void DigitalHeightControl::down()
{
	m_up.open();
	m_down.close();
}

void DigitalHeightControl::idle()
{
	m_up.open();
	m_down.open();
}

/*********************** RED GREEN LED *****************************/

RedGreenLed::RedGreenLed(int redPin, int greenPin, int minOutput, int maxOutput)
  : m_redLed(redPin, minOutput, maxOutput),
    m_greenLed(greenPin, minOutput, maxOutput) { }

void RedGreenLed::intensity(double redFraction, double greenFraction)
{
  m_redLed.intensity(redFraction);
  m_greenLed.intensity(greenFraction);
}

void RedGreenLed::intensity(int redValue, int greenValue)
{
  m_redLed.outputPortValue(redValue);
  m_greenLed.outputPortValue(greenValue);
}

void RedGreenLed::red()
{
  intensity(m_redLed.getMaxOutputValue(), m_greenLed.getMinOutputValue());
}

void RedGreenLed::green()
{
  intensity(m_redLed.getMinOutputValue(), m_greenLed.getMaxOutputValue());
}

void RedGreenLed::orange()
{
  intensity(m_redLed.getMaxOutputValue(), m_greenLed.getMaxOutputValue());
}

void RedGreenLed::on()
{
  intensity(m_redLed.getMaxOutputValue(), m_greenLed.getMaxOutputValue());
}

void RedGreenLed::off()
{
  intensity(m_redLed.getMinOutputValue(), m_greenLed.getMinOutputValue());
}

/************************** RGB LED ********************************/

RGBLed::RGBLed(int redPin, int greenPin, int bluePin, int minOutput, int maxOutput)
  : m_redLed(redPin, minOutput, maxOutput),
    m_greenLed(greenPin, minOutput, maxOutput),
    m_blueLed(bluePin, minOutput, maxOutput) { }

void RGBLed::intensity(double redFraction, double greenFraction, double blueFraction)
{
  m_redLed.intensity(redFraction);
  m_greenLed.intensity(greenFraction);
  m_blueLed.intensity(blueFraction);
}

void RGBLed::intensity(int redValue, int greenValue, int blueValue)
{
  m_redLed.outputPortValue(redValue);
  m_greenLed.outputPortValue(greenValue);
  m_blueLed.outputPortValue(blueValue);
}

void RGBLed::red()
{
  intensity(m_redLed.getMaxOutputValue(), m_greenLed.getMinOutputValue(), m_blueLed.getMinOutputValue());
}

void RGBLed::orange()
{
  int greenAverage = (m_greenLed.getMinOutputValue() + m_greenLed.getMaxOutputValue()) / 2;
  intensity(m_redLed.getMaxOutputValue(), greenAverage, m_blueLed.getMinOutputValue());
}

void RGBLed::yellow()
{
  intensity(m_redLed.getMaxOutputValue(), m_greenLed.getMaxOutputValue(), m_blueLed.getMinOutputValue());
}

void RGBLed::green()
{
  intensity(m_redLed.getMinOutputValue(), m_greenLed.getMaxOutputValue(), m_blueLed.getMinOutputValue());
}

void RGBLed::blue()
{
  intensity(m_redLed.getMinOutputValue(), m_greenLed.getMinOutputValue(), m_blueLed.getMaxOutputValue());
}

void RGBLed::indigo()
{
  intensity(m_redLed.getMinOutputValue(), m_greenLed.getMaxOutputValue(), m_blueLed.getMaxOutputValue());
}

void RGBLed::purple()
{
  int redAverage = (m_redLed.getMinOutputValue() + m_redLed.getMaxOutputValue()) / 2;
  intensity(redAverage, m_greenLed.getMinOutputValue(), m_blueLed.getMaxOutputValue());
}

void RGBLed::white()
{
  intensity(m_redLed.getMaxOutputValue(), m_greenLed.getMaxOutputValue(), m_blueLed.getMaxOutputValue());
}

void RGBLed::on()
{
  intensity(m_redLed.getMaxOutputValue(), m_greenLed.getMaxOutputValue(), m_blueLed.getMaxOutputValue());
}

void RGBLed::off()
{
  intensity(m_redLed.getMinOutputValue(), m_greenLed.getMinOutputValue(), m_blueLed.getMinOutputValue());
}

/*********************** ACTIVE BUZZER *****************************/

ActiveBuzzer::ActiveBuzzer(int pinNumber, int onState)
  : DigitalOutput(pinNumber, onState) { }

void ActiveBuzzer::alert()
{
  beep(200);
  delay(200);
}
  
void ActiveBuzzer::beep(int durationMilliSeconds)
{
  on();
  delay(durationMilliSeconds);
  off();
}

/********************** PASSIVE BUZZER *****************************/

PassiveBuzzer::PassiveBuzzer(int pinNumber)
  : m_pinNumber(pinNumber)
  {
    pinMode(m_pinNumber, OUTPUT);
  }
    
void PassiveBuzzer::tone(int cyclesPerSecond, int durationMilliSeconds)
{
    ::tone(m_pinNumber, cyclesPerSecond);
    delay(durationMilliSeconds);
    noTone(m_pinNumber);
}

/************************ PWM BUZZER *******************************/

PWMBuzzer::PWMBuzzer(int pinNumber)
  : AnalogOutput(pinNumber) { off(); }
  
void PWMBuzzer::on()  { outputPortValue( ( getMinOutputValue() + getMaxOutputValue() ) / 2 ); }

void PWMBuzzer::off() {  outputPortValue(getMinOutputValue()); }

void PWMBuzzer::alert()
{
  beep(200);
  delay(200);
}

void PWMBuzzer::beep(int durationMilliSeconds)
{
  on();
  delay(durationMilliSeconds);
  off();
}

/******************** ANALOG INPUT VOLTAGE *************************/

AnalogInputVoltage::AnalogInputVoltage
  (int pinNumber, int minValue, int maxValue, double minVoltage, double maxVoltage)
  : AnalogInput(pinNumber, minValue, maxValue),
    m_minVoltage(minVoltage),
    m_maxVoltage(maxVoltage)
    { }

double AnalogInputVoltage::voltage() const
{
  return m_minVoltage + (m_maxVoltage - m_minVoltage) * input();
}

/******************** THRESHHOLD SWITCH ****************************/

ThreshHoldSwitch::ThreshHoldSwitch
  (const AnalogInput& analogInput, int threshHold, int closedState)
  : m_analogInput(analogInput),
    m_threshHold(threshHold),
    m_closedState(closedState) { }
    
ThreshHoldSwitch::ThreshHoldSwitch
  (const AnalogInput& analogInput, double threshHoldFraction, int closedState)
  : m_analogInput(analogInput),
    m_closedState(closedState)
  {
    int minInput = m_analogInput.getMinInputValue();
    int maxInput = m_analogInput.getMaxInputValue();
    
    if ( threshHoldFraction < 0.0 ) threshHoldFraction = 0.0;
    if ( threshHoldFraction > 1.0 ) threshHoldFraction = 1.0;
      
    m_threshHold = m_analogInput.getMinInputValue();
      
    if ( minInput <= maxInput )
    {
      // Input value increases from minInput to maxInput
      m_threshHold += (int) ((maxInput - minInput) * threshHoldFraction);
    }
    else
    {
      // Output value decreases from minInput to maxInput
      m_threshHold -= (int) ((minInput - maxInput) * threshHoldFraction);
    }
  }
    
bool ThreshHoldSwitch::open() const { return !closed(); }

bool ThreshHoldSwitch::closed() const
{
  if ( m_closedState == HIGH )
    return ( m_analogInput.inputPortValue() >= m_threshHold );
  else
    return ( m_analogInput.inputPortValue() < m_threshHold );
}

/********************* HUMITURE SENSOR *****************************/

HumitureSensor::HumitureSensor(int pinNumber)
  : m_pinNumber(pinNumber),
    m_dht() { }
    
bool HumitureSensor::sample()
{
  int status = m_dht.read11(m_pinNumber);
  
  switch ( status )
  {
    case DHTLIB_OK:
    return true;
    
    case DHTLIB_ERROR_CHECKSUM:
    case DHTLIB_ERROR_TIMEOUT:
    return false;
  }
}

double HumitureSensor::humidity()
{
  return m_dht.humidity;
}

double HumitureSensor::temperature()
{
  return m_dht.temperature;
}

/************************ LCD2LINE *********************************/

LCD2Line::LCD2Line()
  : m_lcd(0x27, 16, 2)  // Set the LCD address to 0x27 for a 16 chars and 2 line display
{ }

void LCD2Line::setup()
{
  m_lcd.init();
  m_lcd.backlight();
  display(0, "                ");
  display(1, "                ");
}
  
void LCD2Line::displayTopLine(String lineText)
{
  display(0, lineText);
}
  
void LCD2Line::displayBottomLine(String lineText)
{
  display(1, lineText);
}
  
void LCD2Line::display(int lineNumber, String lineText)
{
  m_lcd.setCursor(0, lineNumber); // Set cursor to beginning of line
  m_lcd.print(lineText);
}

/********************* SERIAL MONITOR ******************************/

SerialMonitor::SerialMonitor(int dataRate)
{
  m_dataRate = dataRate;
}

void SerialMonitor::setup()
{
  Serial.begin(m_dataRate);
}

void SerialMonitor::sendIntegerValue(int value)
{
  Serial.print(value, DEC);   // Print value in decimal
}

void SerialMonitor::sendBooleanValue(bool value)
{
  if ( value)
  {
    sendText("true");
  }
  else
  {
    sendText("false");
  }
}

void SerialMonitor::sendDoubleValue(double value, int decimals)
{
  sendText(String(value, decimals));
}

void SerialMonitor::sendDigitalLogicValue(int value)
{
  if ( value == HIGH )
  {
    sendText("HIGH");
  }
  else
  {
    if ( value == LOW )
    {
      sendText("LOW");
    }
    else
    {
      sendText("UNKOWN LOGIC VALUE (");
      sendIntegerValue(value);
      sendText(")");
    }
  }  
}

void SerialMonitor::sendText(String text)
{
  Serial.print(text);
}

void SerialMonitor::sendNewline()
{
  Serial.println();
}

}   // End "namespace sfu"