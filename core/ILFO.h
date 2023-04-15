//
//  ILFO.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 21.06.22.
//

#ifndef ILFO_h
#define ILFO_h

class ILFO {
public:
    virtual void setPeriod(float period) = 0;
    virtual void setFramesPerMeasure(long frames) = 0;
    virtual float triangleValue() = 0;
    virtual void advance() = 0;
};

#endif /* ILFO_h */
