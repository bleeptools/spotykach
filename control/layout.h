#pragma once

//#define ROEY_LAYOUT

/*
Actual layout is defined here:
 Knobs:             control/knob.h ->               daisy::Pin pin(Knob::Target usage) const
 Channel Toggles:   control/channeltoggles.h ->     daisy::Pin pin(Target t, Channel ch)
 Global Toggles:    control/globaltoggles.h ->      daisy::Pin pin(Target t)
 Sensors:           controls/descrete.sensor.h ->   void initialize()
 LEDs:              control/leds.h ->               void initialize()
*/
