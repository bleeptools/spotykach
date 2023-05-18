#include "sync.h"
#include "layout.h"
#include "../common/fcomp.h"

using namespace blptls;
using namespace spotykach;

const float kTempoMin = 30;
const float kTempoMax = 250;

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

void Sync::advance(uint32_t by_samples) {
    _sample_count += by_samples;
    if (_sample_count >= _samples_per_tick) {
        _sample_count %= _samples_per_tick;
        _core->pulse();
    }
}

void Sync::pull(daisy::DaisySeed& hw) {
    auto new_state = g.Read();
    if (_last_state && !new_state) {
        clock_in_tick();
    }
    _last_state = new_state;
}

float Sync::tempo() {
    return _tempo;
}

void Sync::set_tempo(float norm_value) {
    if (fcomp(norm_value, _raw_tempo)) return;
    _raw_tempo = norm_value;
    _tempo = (kTempoMax - kTempoMin) * norm_value + kTempoMin;
    _samples_per_tick = static_cast<uint32_t>(round(static_cast<float>(kSamplesPerTickKof) / _tempo));
}

void Sync::reset() {
    _ptime = 0;
    _wndw.fill(0);
    _iterator = 0;
    _filled = false;
    _dev_cnt = 0;
}

void Sync::clock_in_tick() {
    auto t = daisy::System::GetNow();
    auto delta = t - _ptime;
    if (_ptime > 0) {
        push(delta);
        _tempo = tempo(avg());
        checkDeviation(delta);
    }
    _ptime = t;

    // _beat.ticks++;
    // if (_beat.ticks == kTicksPerBeat) {
    //     _beat.ticks = 0;
    //     _beat.beats ++;
    // }
    // _core->pulse();
}

void Sync::push(uint32_t interval) {
    _wndw[_iterator] = interval;
    _iterator ++;
    _iterator %= _wndw.size(); 
    if (_iterator == 0) {
        _filled = true;
    }
}

float Sync::avg() {
    float s = 0;
    auto l = _filled ? _wndw.size() : _iterator;
    for (size_t i = 0; i < l; i++) s += _wndw[i];
    _avg = s / l;
    return _avg;
}

float Sync::tempo(float tick) {
    return round(15000.f / tick);
}

void Sync::checkDeviation(uint32_t delta) {
    if (delta > _avg - _dev_thres && delta < _avg + _dev_thres) {
        _dev_cnt = 0;
        return;
    }
    if (++_dev_cnt < _dev_cnt_thres) return;
    
    _iterator = 0;
    _filled = false;
    _dev_cnt = 0;
}
