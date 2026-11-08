/*

      HEI to 22re code
      
The ecu for the 22re and Chevy's Hei igniter have send and recieve very similar signals. This code
determines what signal is reliable, IGT or NE, and sends 5v to the advance pin on the Hei. That will
switch it from using the signal it generates to make spark to the ecu signal. This code also generates
an IGF signal for the Toyota ecu to fire the injectors.

Created in 2026
Paul Dueck

*/
#include "MultiMap.h"
#include "PinChangeInterrupt.h"
const int IGF = 1;      // IGF output pin
const int NE = 3;       // NE input pin
const int IGT = 4;      // IGT input pin
const int HEITIME = 0;  // HEI timing ignore pin?

volatile bool IGF_trigger;        // Triggers IGF pulse
bool IGT_hap;                     // IGT State
byte IGT_times = 3;               // How many times IGT needs to come for Hei to switch to that signal.
volatile byte IGT_keep;           // Keeps track of how many times in succession IGT has been triggered.
volatile unsigned long IGT_last;  // Last time IGT was validated
const int IGT_drop = 300;         // Time before switch to NE
const int IGF_time = 1200;        // Duration of the IGF
unsigned long IFG_micros;         // To keep time


void setup() {
  pinMode(IGF, OUTPUT);
  pinMode(HEITIME, OUTPUT);
  pinMode(NE, INPUT_PULLUP);
  pinMode(IGT, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(IGT), IGT_sense, FALLING);
  attachPCINT(digitalPinToPCINT(NE), NE_sense, FALLING);
  delay(20);  // Let the Arduino settle before work
}

void loop() {
  // IGF generation
  if (IGF_trigger == 1) {  //If the ISRs have been triggered, set output to high and reset the LOW timer
    digitalWrite(IGF, LOW);
    IFG_micros = micros();
    IGF_trigger = 0;
  }
  if (micros() - IFG_micros >= IGF_time) {  //If LOW timer has been reset, wait for set time then go LOW
    digitalWrite(IGF, HIGH);
  }

  // Hei IGT input switching
  if (IGT_keep >= IGT_times) {
    digitalWrite(HEITIME, HIGH);
  } else {
    digitalWrite(HEITIME, LOW);
  }

  if (millis() >= IGT_last + IGT_drop) {
    IGT_keep = 0;
    IGT_hap = 0;
  }
}

void IGT_sense() {  //IGT isr
  IGT_keep = IGT_keep + 1;
  IGT_calculate(IGT_keep, IGT_last, IGT_hap);
  if (IGT_hap = 1) {
    IGF_trigger = 1;
  }
}

void NE_sense() {  //NE isr
  if (IGT_hap = 1) {
    return;
  } else {
    IGF_trigger = 1;
  }
}

void IGT_calculate(int num, unsigned long lasttime, bool reliable) {
  if (num >= 3) {
    lasttime = millis;
    num = 0;
    reliable = 1;
  }
}