#include "Engine.h"
#include "globals.h"
#include "../common/fcomp.h"
#include <algorithm>


using namespace vlly;
using namespace spotykach;

inline int gridStepCount(Grid grid) {
    switch (grid) {
        case Grid::even : return EvenStepsCount;
        case Grid::c_word: return CWordsCount;
    }
}

inline float exp_val(float val) {
    return (pow(10.0, 2 * val - 1.0)) / 10.0 - 0.01;
}

Engine::Engine(ITrigger& t, ISource& s, IEnvelope& e, IGenerator& g, ILFO& l):
    _trigger    { t },
    _source     { s },
    _envelope   { e },
    _generator  { g },
    _jitterLFO  { l },
    _is_playing { false },
    _tempo      { 0 },
    _step       { 0 },
    _invalidate_crossfade { false }
{
    _trigger.on_pattern_changed([this](uint32_t step){ 
        this->_step = step;
        this->_invalidate_crossfade = true; 
    });

    setSliceLength(0.5);
    setReverse(false);
    setJitterAmount(0);
    setJitterRate(0.75);
    setFrozen(true);
}

void Engine::setSlicePosition(float normVal) {
    if (fcomp(_raw.slicePosition, normVal)) return;
    _raw.slicePosition = normVal;
    float start = std::min(normVal, 127.f/128.f);
    if (start != _start) {
        _start = start;
        _generator.set_slice_position(_start);
    }
}

void Engine::setSliceLength(float normVal) {
    if (fcomp(_raw.sliceLength, normVal)) return;
    _raw.sliceLength = normVal;
    float slice = fmax(normVal, 1./128.);
    if (slice != _slice) {
        _slice = slice;
        _generator.set_slice_length(exp_val(_slice));
        _invalidate_crossfade = true;
    }
}

void Engine::setJitterAmount(float value) {
    _generator.set_jitter_amount(value);
}

void Engine::setJitterRate(float value) {
    _jitterLFO.setPeriod(1. - value);
}

void Engine::setReverse(bool value) {
    if (value == _raw.reverse) return;
    _raw.reverse = value;
    _generator.set_reverse(value);
}

void Engine::setFrozen(bool frozen) {
    auto isTurningOff = _raw.frozen && !frozen;
    _raw.frozen = frozen;
    _source.set_frozen(frozen);
    if (isTurningOff) {
        _generator.set_cycle_start();
        _generator.set_needs_reset_slices();
    }
}

void Engine::setAntifreeze(bool value) {
    _source.set_antifreeze(value);
}

void Engine::set_pitch_shift(float value) {
    _generator.set_pitch_shift(value);
}

void Engine::set_on_slice(SliceCallback f) {
    _generator.set_on_slice(f);
}

void Engine::initialize() {
    _source.initialize();
    _generator.initialize();
}

void Engine::preprocess(PlaybackParameters p) {
    static uint32_t framesPerMeasure = 0;

    if (!fcomp(p.tempo, _tempo)) {
        _tempo = p.tempo;
        framesPerMeasure = static_cast<uint32_t>(kSecondsPerMinute * p.sampleRate * kBeatsPerMeasure / p.tempo);
        _generator.set_frames_per_measure(framesPerMeasure);
        _jitterLFO.setFramesPerMeasure(framesPerMeasure);
        _invalidate_crossfade = true;
    }

    if (_invalidate_crossfade) {
        auto framesPerStep { static_cast<uint32_t>(static_cast<float>(_step) * framesPerMeasure / (kTicksPerBeat * kBeatsPerMeasure)) };
        auto framesPerSlice { _generator.frames_per_slice() };
        _envelope.setFramesPerCrossfade(std::max(framesPerSlice - framesPerStep, uint32_t(0)));
        _invalidate_crossfade = false;
    }
}

void Engine::set_is_playing(bool value, bool clean) {
    _is_playing = value;
}

void Engine::step(bool engaged) {
    if (!_is_playing) return;
    _trigger.next(engaged);
}

void Engine::process(float in0, float in1, float* out0, float* out1, bool continual, bool reverse) {
    _jitterLFO.advance();
    _source.write(in0, in1);
    _generator.generate(out0, out1, continual, reverse);
}

void Engine::reset(bool hard) {
    if (hard) clear_buffer();
    _generator.reset();
    _trigger.reset();
}

void Engine::clear_buffer() {
    _source.reset();
    _generator.reset();
    _generator.set_cycle_start();
}
