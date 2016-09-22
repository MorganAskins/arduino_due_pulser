#include <Arduino.h>

#define _BV(bit) (1 << (bit) )

int global_cycles = 1;
int global_delay = 1000000;

void ToggleState(bool state)
{
  if(!state)
    TC0->TC_CHANNEL[0].TC_CMR = 0;
  else
    InitTimer();
}

void InitPIO()
{
  // Using PortB.Pin25, which is DUE pin 2
  // Enable the clock for that line.
  PMC->PMC_PCER0 = _BV(ID_PIOB);

  // Disable PIO Control on PB25 and set up for Peripheral B TIOA0
  PIOB->PIO_PDR = PIO_PB25;
  PIOB->PIO_ABSR = PIO_PB25;

  // Enable output on B25
  PIOB->PIO_OER = PIO_PB25;
}

void InitTimer()
{
  // Clock2 is Master clock (MCK) / 8

  // Enable clock for Timer 0
  PMC->PMC_PCER0 = _BV(ID_TC0);

  TC0->TC_CHANNEL[0].TC_CMR =
    TC_CMR_WAVE |
    TC_CMR_TCCLKS_TIMER_CLOCK1 |    // MCK / 2 (can i change this)?
    TC_CMR_WAVSEL_UP_RC |
    TC_CMR_ACPA_SET |
    TC_CMR_ACPC_CLEAR ;

  // Set compare value in Register A and C
  TC0->TC_CHANNEL[0].TC_RA = global_delay;  // Duty cycle
  TC0->TC_CHANNEL[0].TC_RC = global_delay + global_cycles;  // Frequency

  // Enable the clock
  TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;

}

void SetFrequency(int freq)
{
  float mck = 84e6;
  float clk = mck / 2;
  float pulseDelaySec = 1.0/freq;
  global_delay = int(clk * pulseDelaySec);
  // Update cycles to match
  SetCycles(global_cycles);
}

void SetCycles(int cycles)
{
  global_cycles = cycles;
  if (TC0->TC_CHANNEL[0].TC_CMR)
    InitTimer();
}

void setup() {
  Serial.begin(9600);
  InitPIO();
  //InitTimer();
  //SetFrequency(1000); // 1kHz
  //SetCycles(2);
}

String inData = "";

void loop() {
  // Wait for commands
  while (!Serial.available()) {}
  
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    inData += inChar;
    if (inChar == '\n')
    {
      Serial.print("Arduino received: ");
      Serial.println(inData);
      execute_if_command(inData);
      inData = "";
    } 
  }
}

void execute_if_command(String inData)
{
  if( inData.startsWith("SetCycles") ){
    // Expectation is a command like: SetCycles 32
    // So we need the SetCycles command word followed by an integer and nothing else
    int lofstring = inData.length();
    int compare = String("SetCycles").length();
    String nstr = inData.substring(compare, lofstring);
    nstr.trim();
    int istr = nstr.toInt();
    SetCycles(istr);
  }
  if( inData.startsWith("SetFrequency") ){
    // Expectation is a command like: SetFrequency 1000
    // So we need the SetCycles command word followed by an integer and nothing else
    int lofstring = inData.length();
    int compare = String("SetFrequency").length();
    String nstr = inData.substring(compare, lofstring);
    nstr.trim();
    int istr = nstr.toInt();
    SetFrequency(istr);
  }
  if( inData.startsWith("Off") )
  {
    ToggleState(false);
  }
  if( inData.startsWith("On") )
  {
    ToggleState(true);
  }
}

