#include <Arduino.h>

#define _BV(bit) (1 << (bit) )

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
  // I see a PMC_PCR with PERIPH_DIV_MCK (value 0)

  int mck = 84e6;
  int t2 = mck / 2;
  // nanoseconds
  float pulseDelaySec = 1e-6;
  int pulseDelay = t2 * pulseDelaySec;
  int pulseWidth = 4; // cycles, so 120 ns

  // Enable clock for Timer 0
  PMC->PMC_PCER0 = _BV(ID_TC0);

  TC0->TC_CHANNEL[0].TC_CMR =
    TC_CMR_WAVE |
    TC_CMR_TCCLKS_TIMER_CLOCK1 |    // MCK / 2 (can i change this)?
    TC_CMR_WAVSEL_UP_RC |
    TC_CMR_ACPA_SET |
    TC_CMR_ACPC_CLEAR ;
  //TC_CMR_CPCDIS |
  //TC_CMR_CPCSTOP;
  //TC_CMR_CPCTRG;

  // Set compare value in Register A and C
  TC0->TC_CHANNEL[0].TC_RA = pulseDelay;  // Duty cycle
  TC0->TC_CHANNEL[0].TC_RC = pulseDelay + pulseWidth;  // Frequency

  // Enable the clock
  TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;

}

void setup() {
  InitPIO();
  InitTimer();
}

void loop() {

}
