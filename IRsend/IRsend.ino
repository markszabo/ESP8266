/*
 */
#define TOPBIT 0x80000000
#define NEC_BITS 32
#define NEC_HDR_MARK	9000
#define NEC_HDR_SPACE	4500
#define NEC_BIT_MARK	560
#define NEC_ONE_SPACE	1690
#define NEC_ZERO_SPACE	560
#define NEC_RPT_SPACE	2250

int IRpin = 2;
int blinkingFrequencyKHz = 38;

void setup()
{
  Serial.begin(9600);
  pinMode(IRpin, OUTPUT);
}

void loop() {
  /*
Received NEC: FF02FD - ON 
Received NEC: FFA25D - blue
Received NEC: FF9A65 - green

ON/OFF: FF02FD
FADE: FFE01F
  */
 /* Serial.println("Turning on");
  irsend.sendNEC(0xFF02D, 24);
  delay(2000);*/
  Serial.println("Fade");
  sendNEC(0x00FFE01F, 36);
  delay(2000);
  Serial.println("Blue");
  sendNEC(0x00FFA25D, 36);
  delay(2000);
  Serial.println("Green");
  sendNEC(0x00FF9A65, 36);
  delay(2000);
}

void sendNEC(unsigned long data, int nbits)
{
  enableIROut(38);
  mark(NEC_HDR_MARK);
  space(NEC_HDR_SPACE);
  for (int i = 0; i < nbits; i++) {
    if (data & TOPBIT) {
      mark(NEC_BIT_MARK);
      space(NEC_ONE_SPACE);
    } 
    else {
      mark(NEC_BIT_MARK);
      space(NEC_ZERO_SPACE);
    }
    data <<= 1;
  }
  mark(NEC_BIT_MARK);
  space(0);
}

void mark(int time) {
  // Sends an IR mark for the specified number of microseconds.
  // The mark output is modulated at the PWM frequency.
  //TIMER_ENABLE_PWM; // Enable pin 3 PWM output
  //if (time > 0) delayMicroseconds(time);
  long beginning = micros();
  while(micros() - beginning < time){
    digitalWrite(IRpin, HIGH);
    delayMicroseconds(13);
    digitalWrite(IRpin, LOW);
    delayMicroseconds(13); //38 kHz -> T = 26.31 microsec (periodic time), half of it is 13
  }
}

/* Leave pin off for time (given in microseconds) */
void space(int time) {
  // Sends an IR space for the specified number of microseconds.
  // A space is no output, so the PWM output is disabled.
  //TIMER_DISABLE_PWM; // Disable pin 3 PWM output
  digitalWrite(IRpin, LOW);
  if (time > 0) delayMicroseconds(time);
}

void enableIROut(int khz) {
  // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
  // The IR output will be on pin 3 (OC2B).
  // This routine is designed for 36-40KHz; if you use it for other values, it's up to you
  // to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
  // TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
  // controlling the duty cycle.
  // There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
  // To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
  // A few hours staring at the ATmega documentation and this will all make sense.
  // See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.

  
  // Disable the Timer2 Interrupt (which is used for receiving IR)
  //TIMER_DISABLE_INTR; //Timer2 Overflow Interrupt
  
  //pinMode(TIMER_PWM_PIN, OUTPUT);
  //digitalWrite(TIMER_PWM_PIN, LOW); // When not sending PWM, we want it low
  
  // COM2A = 00: disconnect OC2A
  // COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
  // WGM2 = 101: phase-correct PWM with OCRA as top
  // CS2 = 000: no prescaling
  // The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
   blinkingFrequencyKHz = khz;
}
