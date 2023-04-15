//
//  LFO.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 21.06.22.
//

#ifndef LFO_h
#define LFO_h

#include "ILFO.h"

class LFO: public ILFO {
public:
    LFO();
    
    void setPeriod(float) override;
    void setFramesPerMeasure(long) override;
    void advance() override;
    float triangleValue() override;
    
private:
    long _frame;
    long _framesPerMeasure;
    float _period;
 };

#endif /* LFO_h */
