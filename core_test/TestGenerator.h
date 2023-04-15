//
//  TestGenerator.h
//  spotycach_core_test
//
//  Created by Vladyslav Lytvynenko on 16.03.22.
//

#ifndef TestGenerator_h
#define TestGenerator_h

#include <gmock/gmock.h>
#include "IGenerator.h"
#include "Parameters.h"

class TestGenerator: public IGenerator {
public:
    MOCK_METHOD(void, adjustBuffers, (long), (override));
    MOCK_METHOD(void, activateSlice, (long, long, long, bool), (override));
    MOCK_METHOD(void, generate, (float*, float*), (override));
    MOCK_METHOD(void, reset, (), (override));
    MOCK_METHOD(void, setDirection, (vlly::spotykach::Direction), (override));
};

#endif /* TestGenerator_h */
