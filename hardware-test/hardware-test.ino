
#include <Wire.h>
#include "Adafruit_MPR121.h"

#define ROEY_LAYOUT

void setup() {
  Serial.begin(115200);

  setupSwitches();
  setupPads();

  // testLED_A();
  // testLED_B();
  // testLED_R();
}

void loop() {
  //testKnobsA();
  //testKonbsB();
  //testKnobsG();
  //testSwitches();
  //testPads();
  delay(200);
}

////////////////////////////////////////
//// KNOBS /////////////////////////////

enum class Knobs {
  #ifdef ROEY_LAYOUT
    SlicePositionA    = A8,
    SliceLengthA      = A7,
    RetriggerA        = A6,
    JitterAmountA     = A9,
    Tempo             = A5,
    VolumeCrossfade   = A11,
    PatternCrossfade  = A4,
    Pitch             = A10,
    SlicePositionB    = A1,
    SliceLengthB      = A0,
    RetriggerB        = A3,
    JitterAmountB     = A2
  #else
    SlicePositionA    = A10,
    SliceLengthA      = A9,
    RetriggerA        = A8,
    JitterAmountA     = A11,
    Tempo             = A7,
    VolumeCrossfade   = A5,
    PatternCrossfade  = A6,
    Pitch             = A4,
    SlicePositionB    = A2,
    SliceLengthB      = A1,
    RetriggerB        = A3,
    JitterAmountB     = A0
  #endif
};

#ifndef K
#define KR(e) 1023 - analogRead((uint32_t)(Knobs::e))
#endif

void testKnobsA() {
  Serial.print("Slice Position A ");
  Serial.println(KR(SlicePositionA));
  Serial.print("Slice Length A ");
  Serial.println(KR(SliceLengthA));
  Serial.print("Retrigger A ");
  Serial.println(KR(RetriggerA));
  Serial.print("Jitter Amount A ");
  Serial.println(KR(JitterAmountA));
}

void testKonbsB() {
  Serial.print("Slice Position B ");
  Serial.println(KR(SlicePositionB));
  Serial.print("Slice Length B ");
  Serial.println(KR(SliceLengthB));
  Serial.print("Retrigger B ");
  Serial.println(KR(RetriggerB));
  Serial.print("Jitter Amount B ");
  Serial.println(KR(JitterAmountB));
}

void testKnobsG() {
  Serial.print("Tempo ");
  Serial.println(KR(Tempo));
  Serial.print("Volume Crossfade ");
  Serial.println(KR(VolumeCrossfade));
  Serial.print("Pattern Crossfade ");
  Serial.println(KR(PatternCrossfade));
  Serial.print("Pitch ");
  Serial.println(KR(Pitch));
}

////////////////////////////////////////
//// SWITCHES //////////////////////////

enum class Switches {
  #ifdef ROEY_LAYOUT
    GridA     = D1,
    GridB     = D14,
    ReverseA  = D2,
    ReverseB  = D7,
    Mutex     = D4,
    Cascade   = D5,
    Split     = D3
  #else
    GridA     = D0,
    GridB     = D10,
    ReverseA  = D1,
    ReverseB  = D26,
    Mutex     = D29,
    Cascade   = D27,
    Split     = D30
  #endif
};

#ifndef S
#define S(p) (uint32_t)Switches::p
#endif

#ifndef SR
#define SR(p) 1 - digitalRead(S(p))
#endif

void setupSwitches() {
  pinMode(S(GridA), INPUT_PULLUP);
  pinMode(S(GridB), INPUT_PULLUP);
  pinMode(S(ReverseA), INPUT_PULLUP);
  pinMode(S(ReverseB), INPUT_PULLUP);
  pinMode(S(Mutex), INPUT_PULLUP);
  pinMode(S(Cascade), INPUT_PULLUP);
  pinMode(S(Split), INPUT_PULLUP);
}

void testSwitches() {
  Serial.print("Grid A ");
  Serial.println(SR(GridA));
  Serial.print("Grid B ");
  Serial.println(SR(GridB));
  Serial.print("Reverse A ");
  Serial.println(SR(ReverseA));
  Serial.print("Reverse B ");
  Serial.println(SR(ReverseB));
  Serial.print("Mutex ");
  Serial.println(SR(Mutex));
  Serial.print("Cascade ");
  Serial.println(SR(Cascade));
  Serial.print("Split ");
  Serial.println(SR(Split));
}

////////////////////////////////////////
//// PADS //////////////////////////////

#ifndef _pin
#define _pin(shift) (1 << shift)
#endif

#ifndef _together
#define _together(a, b) (a | b)
#endif

enum class Pad {
  #ifdef ROEY_LAYOUT
    PlayStop      = _pin(4),
    OneShotFwdA   = _pin(3),
    OneShotRevA   = _pin(2),
    RecordA       = _together(_pin(3), _pin(2)),
    PatternMinusA = _pin(6),
    PatternPlusA  = _pin(1),
    OneShotFwdB   = _pin(5),
    OneShotRevB   = _pin(0),
    RecordB       = _together(_pin(5), _pin(0)),
    PatternMinusB = _pin(8),
    PatternPlusB  = _pin(9)
  #else
    PlayStop      = _pin(7),
    OneShotFwdA   = _pin(6),
    OneShotRevA   = _pin(4),
    RecordA       = _together(_pin(4), _pin(6)),
    PatternMinusA = _pin(3),
    PatternPlusA  = _pin(2),
    OneShotFwdB   = _pin(9),
    OneShotRevB   = _pin(10),
    RecordB       = _together(_pin(9), _pin(10)),
    PatternMinusB = _pin(8),
    PatternPlusB  = _pin(5)
  #endif
};

Adafruit_MPR121 cap = Adafruit_MPR121();

void setupPads() {
   if (!cap.begin(0x5A)) {
      Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
}

Pad pads[11] = { 
  Pad::PlayStop, 
  Pad::OneShotFwdA, 
  Pad::OneShotRevA, 
  Pad::RecordA, 
  Pad::PatternMinusA, 
  Pad::PatternPlusA, 
  Pad::OneShotFwdB, 
  Pad::OneShotRevB, 
  Pad::RecordB, 
  Pad::PatternMinusB, 
  Pad::PatternPlusB 
};

void testPads() {
  auto state = cap.touched();
  for (auto p: pads) {
    if ((state & (int)p) == (int)p) {
      switch (p) {
        case Pad::PlayStop: Serial.println("Play / Stop"); break;
        case Pad::OneShotFwdA: Serial.println("Fwd A"); break;
        case Pad::OneShotRevA: Serial.println("Rev A"); break;
        case Pad::RecordA: Serial.println("Rec A"); break;
        case Pad::PatternMinusA: Serial.println("Pattern minus A"); break;
        case Pad::PatternPlusA: Serial.println("Pattern plus A"); break;
        case Pad::OneShotFwdB: Serial.println("Fwd B"); break;
        case Pad::OneShotRevB: Serial.println("Rev B"); break;
        case Pad::RecordB: Serial.println("Rec B"); break;
        case Pad::PatternMinusB: Serial.println("Pattern minus B"); break;
        case Pad::PatternPlusB: Serial.println("Pattern plus B"); break;
      }
    }
  }  
}

////////////////////////////////////////
//// LEDS //////////////////////////////

void testLED_A() {
#ifdef ROEY_LAYOUT  
  pinMode(D29, OUTPUT);
  digitalWrite(D29, 1);
#else
  pinMode(D4, OUTPUT);
  digitalWrite(D4, 1);
#endif
}

void testLED_B() {
#ifdef ROEY_LAYOUT  
  pinMode(D26, OUTPUT);
  digitalWrite(D26, 1);
#else
  pinMode(D6, OUTPUT);
  digitalWrite(D6, 1);
#endif
}

void testLED_R() {
#ifdef ROEY_LAYOUT  
  pinMode(D27, OUTPUT);
  digitalWrite(D27, 1);
#else
  pinMode(D5, OUTPUT);
  digitalWrite(D5, 1);
#endif
}
