//
//  TriggerTest.cpp
//  spotycach_core_test
//
//  Created by Vladyslav Lytvynenko on 16.03.22.
//

#include <stdio.h>
#include <gtest/gtest.h>
#include "Engine.h"
#include "Trigger.h"
#include "TestGenerator.h"
#include "TestSource.h"
#include "TestEnvelope.h"

using namespace ::testing;

class TriggerTest: public Test {
protected:
    Trigger *_trig;
    TestGenerator *_gen;
    TestSource *_src;
    TestEnvelope *_env;
    
    void SetUp() override {
        _gen = new TestGenerator();
        _src = new TestSource();
        _env = new TestEnvelope();
        _trig = new Trigger(*_gen);
    }
    
    void TearDown() override {
        delete _trig;
        delete _gen;
        delete _src;
        delete _env;
    }
    
    void doTriggerTest(float shift, float step, float start, int pointsCount, int maxRepeats = 0, int patterns = 1) {
        float framesPerMeasure { 88200 };
        int buffer { 512 };
        int numerator = 4;
        _trig->reset();
        
        _trig->prepareMeterPattern(step, shift, numerator, 4);
        
        _trig->setStart(start);
        
        EXPECT_CALL(*_gen, adjustBuffers);
        _trig->measure(120, 44100, buffer);
        
        EXPECT_CALL(*_env, setFramesPerCrossfade);
        _trig->setSlice(0.5, *_env);
        
        int count { pointsCount };
        if (maxRepeats && maxRepeats <= pointsCount) {
            _trig->setRepeats(maxRepeats);
            count = maxRepeats;
        }
        long onset = 0;
        long pickupOffset = start * framesPerMeasure;
        long framesPerSlice = step * framesPerMeasure;
        EXPECT_CALL(*_gen, activateSlice(onset, pickupOffset, framesPerSlice, false)).Times(patterns * count);
        
        float currentBeat { 0 };
        float beatIncrement = 4 * buffer / framesPerMeasure;
        bool isLaunch = true;
        do {
            _trig->schedule(currentBeat, isLaunch);
            isLaunch = false;
            for (int i = 0; i < buffer; i++) {
                _trig->next(true);
            };
            currentBeat += beatIncrement;
        } while (currentBeat < patterns * _trig->beatsPerPattern() - beatIncrement);
    }
    
    void switchToTriplet8Playing() {
        std::vector<float> p = { 0.000000, 0.166667, 0.333333, 0.500000, 0.666667, 0.833333, 1.000000, 1.166667, 1.333333, 1.500000, 1.666667, 1.833333, 2.000000, 2.166667, 2.333333, 2.500000, 2.666667, 2.833333, 3.000000, 3.166667, 3.333333, 3.500000, 3.666667, 3.833333 };
        float bt = 1.75;
        int npi = 3;
        adjustNextIndex(p, npi, bt, false);
        
        EXPECT_EQ(npi, 11);
    }
    
    void switchToStraight4() {
        std::vector<float> p = { 0.000000, 1.000000, 2.000000, 3.000000 };
        float bt = 1.75;
        int npi = 3;
        adjustNextIndex(p, npi, bt, false);
        
        EXPECT_EQ(npi, 2);
    }
    
    void switchToStraight8Playing() {
        std::vector<float> p = { 0.000000, 0.500000, 1.000000, 1.500000, 2.000000, 2.500000, 3.000000, 3.500000 };
        float bt = 1.3;
        int npi = 2;
        adjustNextIndex(p, npi, bt, false);
        
        EXPECT_EQ(npi, 3);
    }
    
    void switchToStraight8Reset() {
        std::vector<float> p = { 0.000000, 0.500000, 1.000000, 1.500000, 2.000000, 2.500000, 3.000000, 3.500000 };
        float bt = 0;
        int npi = 2;
        adjustNextIndex(p, npi, bt, true);
        
        EXPECT_EQ(npi, 0);
    }
    
    void switchToPoint8Reset() {
        std::vector<float> p = { 0.000000, 0.750000, 1.500000, 2.250000, 3.000000, 3.750000, 4.500000, 5.250000, 6.000000, 6.750000, 7.500000, 8.250000, 9.000000, 9.750000, 10.500000, 11.250000 };
        float bt = 0;
        int npi = 0;
        adjustNextIndex(p, npi, bt, true);
        
        EXPECT_EQ(npi, 0);
    }
    
    void switchToWhole() {
        std::vector<float> p = { 0.000000 };
        float bt = 2.645720;
        int npi = 3;
        adjustNextIndex(p, npi, bt, false);
        
        EXPECT_EQ(npi, 0);
    }
};
//                                                      shf         stp         sta         pts     rps       pat
TEST_F(TriggerTest, Straight1_Start0)   { doTriggerTest(0,          1.,         0,          1,      0,      2); }
TEST_F(TriggerTest, Straight05_Start0)  { doTriggerTest(0,          1./2.,      0,          2,      0,      2); }
TEST_F(TriggerTest, Straight8_Start0)   { doTriggerTest(0,          1./8.,      0,          8,      0,      2); }
TEST_F(TriggerTest, Straight8)          { doTriggerTest(2./16.,     1./8.,      1.5/8.,     5,      5,      2); }
TEST_F(TriggerTest, Straight8_3Times)   { doTriggerTest(2./16.,     1./8.,      1.5/8.,     7,      3        ); }
TEST_F(TriggerTest, Straight8_6Times)   { doTriggerTest(2./16.,     1./8.,      1.5/8.,     7,      6,      1); }
TEST_F(TriggerTest, Point8)             { doTriggerTest(4./16.,     1.5/8.,     1./4.,      15               ); }
TEST_F(TriggerTest, Point8_8Times)      { doTriggerTest(1./16.,     1.5/8.,     1./4.,      15,     8        ); }
TEST_F(TriggerTest, Triplet8)           { doTriggerTest(0,          1./24.,     1./8.,      24               ); }
TEST_F(TriggerTest, Triplet8_10Times)   { doTriggerTest(2./16.,     1./24.,     1./8.,      24,     10       ); }

TEST_F(TriggerTest, toTriplet8Playing)  { switchToTriplet8Playing(); };
TEST_F(TriggerTest, toStraight4)        { switchToStraight4(); };
TEST_F(TriggerTest, toStraight8Playing) { switchToStraight8Playing(); };
TEST_F(TriggerTest, toStraight8Reset)   { switchToStraight8Reset(); };
TEST_F(TriggerTest, toPoint8Reset)      { switchToPoint8Reset(); };
TEST_F(TriggerTest, toWholeReset)       { switchToWhole(); };
