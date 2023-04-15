#include <gtest/gtest.h>
#include "Generator.h"
#include "Source.h"
#include "SliceBuffer.h"
#include "TestEnvelope.h"

using namespace ::testing;

class GeneratorTest: public Test {
protected:
    Generator *_gen;
    Source* _src;
    TestEnvelope* _env;
    
    void SetUp() override {
        _src = new Source();
        _src->reset();
        
        _env = new TestEnvelope();
        
        _gen = new Generator(*_src, *_env);
    }
    
    void TearDown() override {
        delete _gen;
        delete _src;
        delete _env;
    }
    
    void doTestGenerateSlice() {
        auto sliceLength = 20;
        _gen->adjustBuffers(50);
        _gen->activateSlice(0, 0, sliceLength, true);
        
        for (auto i = 0; i < 100; i++) {
            float out0 = 0;
            float out1 = 0;
            _src->write(i, i);
            
            if (i < sliceLength) {
                EXPECT_CALL(*_env, attackLength);
                EXPECT_CALL(*_env, decayLength);
            }
            _gen->generate(&out0, &out1);
            if (i < sliceLength) {
                EXPECT_EQ(out0, i);
                EXPECT_EQ(out1, i);
            }
        }
    }
    
    void doTestGenerateSliceOffset() {
        auto sliceLength = 8;
        auto offset = 5;
        _src->setMode(SourceMode::flow);
        _gen->adjustBuffers(20);
        float out0 = 0;
        float out1 = 0;
        for (auto i = 0; i < 20; i++) {
            _src->write(i, i);
            if (i == offset) {
                _gen->activateSlice(0, offset, sliceLength, true);
            }
            
            
            if (i >= offset && i < sliceLength + offset) {
                EXPECT_CALL(*_env, attackLength);
                EXPECT_CALL(*_env, decayLength);
            }
            _gen->generate(&out0, &out1);
            if (i >= offset && i < sliceLength + offset) {
                EXPECT_EQ(out0, i);
                EXPECT_EQ(out1, i);
            }
        }
    }
    
    void doTestGenerateSliceOffsetFlow() {
        auto sliceLength = 8;
        auto offset = 5;
        _src->setMode(SourceMode::flow);
        _gen->adjustBuffers(10);
        float out0 = 0;
        float out1 = 0;
        for (auto i = 0; i < 30; i++) {
            _src->write(i, i);
            if (i == offset) {
                _gen->activateSlice(0, offset, sliceLength, true);
            }
            
            
            if (i >= offset && i < sliceLength + offset) {
                EXPECT_CALL(*_env, attackLength);
                EXPECT_CALL(*_env, decayLength);
            }
            _gen->generate(&out0, &out1);
            if (i >= offset && i < sliceLength + offset) {
                EXPECT_EQ(out0, i);
                EXPECT_EQ(out1, i);
            }
        }
    }
};

TEST_F(GeneratorTest, doTestGenerateSlice) { doTestGenerateSlice(); }
TEST_F(GeneratorTest, doTestGenerateSliceOffset) { doTestGenerateSliceOffset(); }
TEST_F(GeneratorTest, doTestGenerateSliceOffsetFlow) { doTestGenerateSliceOffsetFlow(); }
