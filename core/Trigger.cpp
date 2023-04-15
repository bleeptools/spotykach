//
//  Trigger.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "Trigger.h"
#include <cmath>
#include <algorithm>
#include "globals.h"
#include "../common/deb.h"

Trigger::Trigger(IGenerator& inGenerator) :
    _generator          { inGenerator },
    _pointsCount        { 0 },
    _iterator           { 0 },
    _nextPointIndex     { 0 },
    _beatsPerPattern    { 0 },
    _ticksTillUnlock    { 0 },
    _repeats            { INT32_MAX },
    _retrigger          { 0 } 
    {}

void Trigger::prepareCWordPattern(int onsets, int shift) {
    _pointsCount = 0;
    _triggerPoints.fill(0);
    const size_t size = 16;
    int y = onsets, a = y;
    int x = size - onsets, b = x;
    std::array<char, size> pattern;
    
    pattern[0] = 1;
    
    size_t i = 1;
    while (a != b) {
        if (a > b) {
            pattern[i] = 1;
            b += x;
        }
        else {
            pattern[i] = 0;
            a += y;
        }
        i++;
    }

    pattern[i] = 0;
    i++;

    if (i < size) {
        const auto offset = i;
        i = 0;
        while (i + offset < size) {
            pattern[i + offset] = pattern[i];
            i++;
        }
    }
    
    _beatsPerPattern = kBeatsPerMeasure;
    
    auto ticks_per_16th = kTicksPerBeat / 4;
    auto ticks_per_pattern = _beatsPerPattern * kTicksPerBeat;
    for (size_t i = 0; i < pattern.size(); i++) {
        if (!pattern[i]) continue;
        auto point = i * ticks_per_16th + shift;
        if (point >= ticks_per_pattern) {
            point -= ticks_per_pattern;
        }
        _triggerPoints[_pointsCount] = point;
        _pointsCount ++;
    }
    adjustIterator();
}

void Trigger::prepareMeterPattern(int step, int shift) {
    _pointsCount = 0;
    _triggerPoints.fill(0);
    int pattern_length { 0 };
    while (pattern_length % kTicksPerBeat || pattern_length < kTicksPerBeat * kBeatsPerMeasure) {
        _triggerPoints[_pointsCount] = pattern_length;
        _pointsCount ++;
        pattern_length += step;
    }
    _beatsPerPattern = pattern_length / kTicksPerBeat;
    uint32_t ticks_per_pattern = _beatsPerPattern * kTicksPerBeat;
    for (uint32_t i = 0; i < _pointsCount; i++) {
        auto point = _triggerPoints[i] + shift;
        if (point >= ticks_per_pattern) {
            point -= ticks_per_pattern;
        }
        _triggerPoints[i] = point;
    }
    adjustIterator();
}

void Trigger::setRetrigger(int retrigger) {
    _retrigger = retrigger;
}

void Trigger::adjustIterator() {
    adjustNextIndex(_triggerPoints.data(), _pointsCount, _iterator, _nextPointIndex);
}

void Trigger::setRepeats(int repeats) {
    _repeats = _pointsCount && repeats > _pointsCount ? _pointsCount : repeats;
}

void Trigger::one_shot(bool reverse) {
    _generator.activate_slice(0, reverse ? -1 : 1);
}

void Trigger::next(bool engaged) {
    if (_ticksTillUnlock > 0) _ticksTillUnlock--;
    if (_iterator == _triggerPoints[_nextPointIndex]) {
        if (engaged && _nextPointIndex < _repeats) {
            long onset = 0;
            if (_retrigger && _nextPointIndex % _retrigger == 0) {
                onset = static_cast<float>(_triggerPoints[_nextPointIndex]) / kTicksPerBeat;
            }
            _generator.activate_slice(onset, 0);
            _ticksTillUnlock = 1;
        }
        _nextPointIndex = (_nextPointIndex + 1) % _pointsCount;
    }
    _iterator = (_iterator + 1) % (_beatsPerPattern * kTicksPerBeat);
}

void Trigger::reset() {
    _iterator = 0;
    _nextPointIndex = 0;
    _ticksTillUnlock = 0;
}
