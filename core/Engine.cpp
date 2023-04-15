//
//  Engine.cpp
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 02.04.22.
//


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
    _grid       { Grid::c_word },
    _pattern_indexes { 6, 4 },
    _onsets     { 7 },
    _step       { 0 },
    _shift      { 0 }
{
    setRepeats(1.0);
    setShift(0);
    set_grid(1);
    setSliceLength(0.5);
    setReverse(false);
    setRetrigger(0);
    setDeclick(true);
    setJitterAmount(0);
    setJitterRate(0.75);
    setFrozen(true);
}

void Engine::setShift(float normVal) {
    if (fcomp(_raw.shift, normVal)) return;
    _raw.shift = normVal;
    float shiftValue = int(normVal * 15);
    _shift = shiftValue;
    prepare_pattern();
}

void Engine::init_pattern_indexes(std::array<int, kGrid_Count> indexes) {
    _pattern_indexes = indexes;
}

int Engine::next_pattern() {
    auto index = _pattern_indexes[int(_grid)];
    return set_pattern_index(++index);
}

int Engine::prev_pattern() {
    auto index = _pattern_indexes[int(_grid)];
    return set_pattern_index(-- index);
}

int Engine::set_pattern_index(int index) {
    auto step = _step;
    auto onsets = _onsets;
    int max_index = _grid == Grid::even ? EvenStepsCount - 1 : CWordsCount - 1; 
    index = std::max(index, 0);
    index = std::min(index, max_index);

    _pattern_indexes[int(_grid)] = index;    

    switch (_grid) {
        case Grid::even: step = EvenSteps[index]; break;
        case Grid::c_word: onsets = CWords[index]; step = 1; /*1/16*/ break;
    }

    if (step != _step || onsets != _onsets) {
        _step = step;
        _onsets = onsets;
        prepare_pattern();
    }

    return index;
}

void Engine::prepare_pattern() {
    if (_grid == Grid::c_word) {
        _trigger.prepareCWordPattern(_onsets, _shift);
    } 
    else {
        _trigger.prepareMeterPattern(_step, _shift);
    }
    _invalidateCrossfade = true;
}

void Engine::set_grid(float normVal) {
    _raw.grid = normVal;
    Grid grid = spotykach::Grid(normVal * (kGrid_Count - 1));
    if (grid != _grid) {
        _grid = grid;
        set_pattern_index(_pattern_indexes[int(_grid)]);
        prepare_pattern();
        setRepeats(_raw.repeats);
    }
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
        _invalidateCrossfade = true;
    }
}

void Engine::setRepeats(float normVal) {
    _raw.repeats = normVal;
    auto rnd = round(normVal * _trigger.pointsCount());
    _trigger.setRepeats(std::max(static_cast<int>(rnd), 1));
}

void Engine::setRetrigger(float normVal) {
    _raw.retrigger = normVal;
    _trigger.setRetrigger(round(normVal * 8));
}

void Engine::setJitterAmount(float value) {
    _raw.jitterAmount = value;
    _generator.set_jitter_amount(value);
}

void Engine::setJitterRate(float value) {
    _jitterLFO.setPeriod(1. - value);
}

void Engine::setDeclick(bool declick) {
    _raw.declick = declick;
    _envelope.setDeclick(declick);
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
        _invalidateCrossfade = true;
    }

    if (_invalidateCrossfade) {
        auto framesPerStep { static_cast<uint32_t>(static_cast<float>(_step) * framesPerMeasure / (kTicksPerBeat * kBeatsPerMeasure)) };
        auto framesPerSlice { _generator.frames_per_slice() };
        _envelope.setFramesPerCrossfade(std::max(framesPerSlice - framesPerStep, uint32_t(0)));
        _invalidateCrossfade = false;
    }
}

void Engine::set_is_playing(bool value, bool clean) {
    //if (clean && value != _is_playing) reset();
    _is_playing = value;
}

void Engine::one_shot(bool reverse) {
    _trigger.one_shot(reverse);
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
    if (hard) {
        _source.reset();
        _generator.set_cycle_start();
    }
        
    _generator.reset();
    _trigger.reset();
}
