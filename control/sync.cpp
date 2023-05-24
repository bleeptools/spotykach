#include "sync.h"
#include "layout.h"
#include "../common/fcomp.h"
#include "../core/globals.h"

using namespace blptls;
using namespace spotykach;

void Sync::run(Clockable& clockable) {
    _clockable = &clockable;
    daisy::GPIO::Config cfg;
#ifdef ROEY_LAYOUT
	cfg.pin = daisy::seed::D0;
#else
    cfg.pin = daisy::seed::D2;
#endif
	g.Init(cfg);
}

// Called by internal interrupt timer (audio callback in this implementation).
void Sync::tick() {
    if (!_is_playing) return;
    sync();
}

/*
Derived from Maximum MIDI Music Applications in C++ by Paul Messick
This method generates internal ticks and also synchronises to the external clock.
So it's called both from internal interrupt timer (audio callback in this implementation) and upon external clock tick reception.
nticks - integer count of internal ticks
_fticks - fractional count of internal ticks
_tempo_ticks - integer ticks count since last external clock
_tempo_mks - tempo in microseconds / beat (quarter note)
_resync - flag to resync to external clock. Is set to true once external clock tick is received.
_hold - flag to stop advancing internal timeline if the number of internal ticks exceeded expected count of internal ticks per extrnal tick
kTRTime - internal resolution (ppqn) multiplied by interrupt interval.
*/
void Sync::sync() {
    uint32_t nticks = 0;

    //If we generated more internal ticks per extrnal tick as expected,
    //we don't advance internal "timeline", but only accumulate _tempo_ticks
    //in order to calculate and correct the tempo.
    //This flag is set to false upon reception of the external tick.
    if (_hold) {
        nticks = (_fticks + kTRtime) / _tempo_mks;
        _fticks += kTRtime - (nticks * _tempo_mks);
        _tempo_ticks += nticks;
        return;
    }

    //Once a tick of the extrnal clock is received,
    //we do resync, i.e. align inernal timeline with the external one
    //and adjust tempo.
    if (_resync) {
        _fticks = 0;
        nticks = kTicksPerClock - (_ticks - _ticks_at_last_clock);
        _ticks_at_last_clock = _ticks + nticks;
        _tempo_mks -= ((int32_t)kTicksPerClock - (int32_t)_tempo_ticks) * (int32_t)_tempo_mks / (int32_t)kPPQN;
        _tempo_ticks = 0;
        _resync = false;
    }
    //Regular mode. We generate internal ticks.
    else {
        nticks = (_fticks + kTRtime) / _tempo_mks;
        _fticks += kTRtime - nticks * _tempo_mks;
        if (_is_synced_to_external_clock) {
            _tempo_ticks += nticks;
            //If there are more internal ticks per the external tick than 
            //expected, we set _hold to true effectively stopping advancing timeline
            //until next external tick
            if (_ticks - _ticks_at_last_clock + nticks >= kTicksPerClock) {
                nticks = kTicksPerClock - 1 - (_ticks - _ticks_at_last_clock);
                _hold = true;
            }
        }
    }

    //Accumulate ticks
    _ticks += nticks;

    //Advance timeline
    for (uint32_t i = 0; i < nticks; i++) _clockable->tick();
}

/*
In case of external clock sync this
method only schedules playback. Actual playback 
starts on the first tick of the external clock.
see clock_in_tick() below.
*/
void Sync::set_is_playing(bool is_playing) {
    if (is_playing == _is_playing) return;
     
    if (is_playing) {
        if (_is_synced_to_external_clock) _is_about_to_play = true;
        else _is_playing = true;
    }
    else {
        _is_playing = false;
        reset();
    }
}

/*
Setting tempo from internal control. 
Has no effect in case of syncing to extrnal clock.
*/
void Sync::set_tempo(float norm_value) {
    if (_is_synced_to_external_clock || fcomp(norm_value, _raw_tempo)) return;
    _raw_tempo = norm_value;
    _tempo = (kTempoMax - kTempoMin) * norm_value + kTempoMin;
    _tempo_mks = tempo_mks(_tempo);
}

void Sync::set_is_synced_to_external_clock(bool value) {
    _is_synced_to_external_clock = value;   
}

/*
Read external clock pin
*/
void Sync::pull(daisy::DaisySeed& hw) {
    auto new_state = g.Read();
    if (_last_state && !new_state) {
        external_clock_tick();
    }
    _last_state = new_state;
}

/*
External clock received
This method starts playback on first received clock
after playback was scheduled. After that, calls sync 
for every tick received.
*/
void Sync::external_clock_tick() {
    if (!_is_playing && !_is_about_to_play) return;

    if (_is_about_to_play) {
        _is_about_to_play = false;
        _is_playing = true;
    }
    else {
        _resync = true;
        _hold = false;
        sync();
    }
}

void Sync::reset() {
    _fticks = 0;
    _ticks = 0;
    _ticks_at_last_clock = 0;
    _tempo_ticks = 0;
    _hold = false;
    _resync = false;
}
