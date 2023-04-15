//
//  Envelope.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/01/15.
//  Copyright (c) 2015 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Envelope__
#define __Spotykach__Envelope__

#include <stdio.h>
#include "IEnvelope.h"

class Envelope: public IEnvelope {
    
public:
    Envelope();
    
    void setDeclick(bool inDeclick) override;
    
    long attackLength() override { return _attackLength; };
    long decayLength() override { return _decayLength; };
    
    void setFramesPerCrossfade(long inFrames) override;
    
    float attackAttenuation(long currentFrame) override;
    float decayAttenuation(long currentFrame) override;

private:
    bool _declick;
    long _attackLength;
    long _decayLength;
    long _framesPerCrossfade;
    
    void measure();
};

#endif /* defined(__Spotykach__Envelope__) */
