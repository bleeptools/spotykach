//
//  IEnvelope.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 14.03.22.
//

#ifndef IEnvelope_h
#define IEnvelope_h

class IEnvelope {
public:
    virtual void setDeclick(bool inDeclick) = 0;
    
    virtual long attackLength() = 0;
    virtual long decayLength() = 0;
    
    virtual void setFramesPerCrossfade(long inFrames) = 0;
    
    virtual float attackAttenuation(long currentFrame) = 0;
    virtual float decayAttenuation(long currentFrame) = 0;
    
    virtual ~IEnvelope() {};
};

#endif /* IEnvelope_h */
