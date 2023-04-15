//
//  Trigger.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Scheduler__
#define __Spotykach__Scheduler__

#include "IGenerator.h"
#include "ITrigger.h"
#include "ILFO.h"
#include <random>

static inline void adjustNextIndex(uint32_t* points, uint32_t pointsCount, uint32_t iterator, uint32_t& nextIndex) {
    int newNextIndex = 0;
    float nextDiff = INT32_MAX;
    for (uint32_t i = 0; i < pointsCount; i++) {
        auto point = points[i];
        if (point >= iterator && point - iterator < nextDiff) {
            nextDiff = point - iterator;
            newNextIndex = i;
        }
    }
    nextIndex = newNextIndex;
}

class Trigger: public ITrigger {
public:
    Trigger(IGenerator& inGenerator);
    
    uint32_t pointsCount() override { return _pointsCount; };
    uint32_t beatsPerPattern() override { return _beatsPerPattern; };
    void prepareMeterPattern(int, int) override;
    void prepareCWordPattern(int, int) override;
    
    void one_shot(bool) override;
    void next(bool) override;
    
    void reset() override;
    
    void setRetrigger(int) override;
    
    uint32_t repeats() override { return _repeats; };
    void setRepeats(int) override;
    
    bool locking() override { return _ticksTillUnlock > 0; };
    
    void set_index(int ndx) {
        index = ndx;
    }
    int index;

private:
    void adjustIterator();
    void adjustRepeatsIfNeeded();

    IGenerator& _generator;

    std::array<uint32_t, 64> _triggerPoints;
    uint32_t _pointsCount;
    uint32_t _iterator;
    uint32_t _nextPointIndex;
    uint32_t _beatsPerPattern;
    uint32_t _framesPerBeat;

    uint32_t _ticksTillUnlock;
    
    uint32_t _repeats;
    uint32_t _retrigger;
};

#endif
