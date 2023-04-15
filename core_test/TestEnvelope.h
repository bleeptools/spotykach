//
//  TestEnvelope.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 31.03.22.
//

#ifndef TestEnvelope_h
#define TestEnvelope_h

#include <gmock/gmock.h>

class TestEnvelope: public IEnvelope {
public:
    MOCK_METHOD(void, setDeclick, (bool), (override));
    MOCK_METHOD(long, attackLength, (), (override));
    MOCK_METHOD(long, decayLength, (), (override));
    MOCK_METHOD(void, setFramesPerCrossfade, (long), (override));
    MOCK_METHOD(float, attackAttenuation, (long), (override));
    MOCK_METHOD(float, decayAttenuation, (long), (override));
};

#endif /* TestEnvelope_h */
