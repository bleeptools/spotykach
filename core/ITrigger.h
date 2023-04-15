//
//  ITrigger.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 16.03.22.
//

#ifndef ITrigger_h
#define ITrigger_h

#include "IEnvelope.h"
#include <algorithm>

class ITrigger {
public:
    virtual void set_index(int index) = 0;

    virtual uint32_t pointsCount() = 0;
    virtual uint32_t beatsPerPattern() = 0;
    virtual void prepareMeterPattern(int step, int shift) = 0;
    virtual void prepareCWordPattern(int onsets, int shift) = 0;
    virtual void one_shot(bool reverse) = 0;
    virtual void next(bool engaged) = 0;
    virtual void reset() = 0;
    
    virtual void setRetrigger(int) = 0;
    
    virtual uint32_t repeats() = 0;
    virtual void setRepeats(int) = 0;
    
    virtual bool locking() = 0;
    
    virtual ~ITrigger() {};
};

#endif /* ITrigger_h */
