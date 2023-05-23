#include "sync.h"
#include "layout.h"
#include "../common/fcomp.h"
#include "../core/globals.h"

using namespace blptls;
using namespace spotykach;

void Sync::run(Core& core) {
    _core = &core;
    daisy::GPIO::Config cfg;
#ifdef ROEY_LAYOUT
	cfg.pin = daisy::seed::D0;
#else
    cfg.pin = daisy::seed::D2;
#endif
	g.Init(cfg);
}

void Sync::tick() {
    if (!_is_playing) return;
    sync();
}

void Sync::sync() {
    uint32_t nticks = 0;

    if (_hold) {
        nticks = (_fticks + kTRtime) / _tempo_mks;
        _fticks += kTRtime - (nticks * _tempo_mks);
        _tempo_ticks += nticks;
        return;
    }

    if (_resync) {
        _fticks = 0;
        nticks = kTicksPerClock - (_ticks - _ticks_at_last_clock);
        _ticks_at_last_clock = _ticks + nticks;
        _tempo_mks -= ((int32_t)kTicksPerClock - (int32_t)_tempo_ticks) * (int32_t)_tempo_mks / (int32_t)kPPQN;
        _tempo_ticks = 0;
        _resync = false;
    }
    else {
        nticks = (_fticks + kTRtime) / _tempo_mks;
        _fticks += kTRtime - nticks * _tempo_mks;
        _tempo_ticks += nticks;
        if (_ticks - _ticks_at_last_clock + nticks >= kTicksPerClock) {
            nticks = kTicksPerClock - 1 - (_ticks - _ticks_at_last_clock);
            _hold = true;
        }
    }

    _ticks += nticks;
    for (uint32_t i = 0; i < nticks; i++) {
        _core->pulse();
    }
}

void Sync::set_is_playing(bool is_playing) {
    if (is_playing == _is_playing) return;
     
    if (is_playing) {
        _is_about_to_play = true;
    }
    else {
        _is_playing = false;
        reset();
    }
}

void Sync::set_tempo(float norm_value) {
    // if (fcomp(norm_value, _raw_tempo)) return;
    // _raw_tempo = norm_value;
    // _tempo = (kTempoMax - kTempoMin) * norm_value + kTempoMin;
    // _tempo_mks = tempo_mks(_tempo);
}

void Sync::pull(daisy::DaisySeed& hw) {
    auto new_state = g.Read();
    if (_last_state && !new_state) {
        clock_in_tick();
    }
    _last_state = new_state;
}

void Sync::clock_in_tick() {
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
}
