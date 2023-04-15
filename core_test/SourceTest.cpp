//
//  SourceTest.cpp
//  spotycach_core_test
//
//  Created by Vladyslav Lytvynenko on 16.05.22.
//

#include <stdio.h>
#include <gtest/gtest.h>
#include "Source.h"

using namespace ::testing;

class SourceTest: public Test {
protected:
    Source *_src;
    
    void SetUp() override {
        _src = new Source();
        _src->reset();
        _src->size(10);
    }
    
    void TearDown() override {
        delete _src;
    }
    
    void doTestBufferNotFilledFreeze() {
        for (int i = 0; i < 8; i++) {
            _src->write(1., 1.);
        }
        
        float l10 = 0;
        float l11 = 0;
        _src->read(l10, l11, 7);
        EXPECT_EQ(l10, 1.);
        EXPECT_EQ(l11, 1.);
        
        float l20 = 0;
        float l21 = 0;
        _src->read(l20, l21, 8);
        EXPECT_EQ(l20, 0);
        EXPECT_EQ(l21, 0);
    }
    
    void doTestBufferFilledFreeze() {
        for (int i = 0; i < 10; i++) {
            float value = i < 5 ? 1. : 2.;
            _src->write(value, value);
        }
        
        float l10 = 0;
        float l11 = 0;
        _src->read(l10, l11, 4);
        EXPECT_EQ(l10, 1.);
        EXPECT_EQ(l11, 1.);
        
        float l20 = 0;
        float l21 = 0;
        _src->read(l20, l21, 9);
        EXPECT_EQ(l20, 2.);
        EXPECT_EQ(l21, 2.);
    }
    
    void doTestFlow() {
        float out0 = 0;
        float out1 = 0;
        _src->setMode(SourceMode::flow);
        for (int i = 0; i < 100; i++) {
            _src->write(i, i);
            _src->read(out0, out1, _src->readHead());
            EXPECT_EQ(out0, float(i));
            EXPECT_EQ(out1, float(i));
        }
    }
    
    void doTestFlowOffset() {
        float out0 = 0;
        float out1 = 0;
        _src->setMode(SourceMode::flow);
        for (int i = 0; i < 100; i++) {
            _src->write(i, i);
            if (i >= 20) {
                _src->read(out0, out1, i + 8); //read at 8-th position of 10-slots buffer
                EXPECT_EQ(out0, float(i - 2)); //8-th position value at this point is 18 so this difference should remain
                EXPECT_EQ(out1, float(i - 2)); //8-th position value at this point is 18 so this difference should remain
            }
        }
    }
};

TEST_F(SourceTest, frontBufferNotFilledFreeze) { doTestBufferNotFilledFreeze(); }
TEST_F(SourceTest, frontBufferFilledFreeze) { doTestBufferFilledFreeze(); }
TEST_F(SourceTest, bufferFlow) { doTestFlow(); }
TEST_F(SourceTest, bufferFlowOffset) { doTestFlowOffset(); }
