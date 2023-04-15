//
//  Slice.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Slice__
#define __Spotykach__Slice__

#include "ISource.h"
#include "ISliceBuffer.h"
#include "IEnvelope.h"
#include "../fx/pitch.shift.h"

class Slice {
public:
    Slice(ISource& inSource, ISliceBuffer& inBuffer, IEnvelope& inEnvelope);
    ~Slice() = default;
    
    
    bool isActive() { return _active; };
    bool isInactive() { return !_active; };
    void initialize();
    void activate(long offset, long length, bool reverse, float pitch, float volume);
    void synthesize(float *out0, float* out1);
    void setNeedsReset();
    
private :
    ISource& _source;
    IEnvelope& _envelope;
    ISliceBuffer& _buffer;
    PitchShift _pitch;

    bool _active;
    
    long _length;
    long _offset;
    long _iterator;
    bool _reverse;
    
    bool _needsReset;
    
    float *_declickIn;
    float *_declickOut;
    float _volume;
    
    void next();
};

#endif
