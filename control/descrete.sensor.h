#pragma once

#include <stdint.h>
#include "dev/mpr121.h"
#include "descrete.sensor.pad.h"
#include "layout.h"

#ifndef _pin
#define _pin(shift) (1 << shift)
#endif

#ifndef _together
#define _together(a, b) (a | b)
#endif

#ifndef _bit
#define _bit(n, v) (v >> n & 1)
#endif

#ifndef _both
#define _both(n, m, v) _bit(n, v) && _bit(m, v)
#endif

#ifndef _one_of
#define _one_of(n, m, v) _bit(n, v) ^ _bit(m, v)
#endif

#ifndef _target_index
#define _target_index(t) static_cast<uint32_t>(t)
#endif

//If 1st or 2nd pad was touched and now both are,
//ignore change, reset state to previous.
inline uint16_t one_or_both(int n, int m, uint16_t v, uint16_t p) {
    if (_one_of(n, m, p) && _both(n, m, v)) {
        v = (v & ~(1 << n)) | (_bit(n, p) << n);
        v = (v & ~(1 << m)) | (_bit(m, p) << m);
    }
    return v;
}

class  DescreteSensor {
public:
    //PADS TARGETS #####################################################
    //
    enum class Target {
        PlayStop,
        OneShotFwdA,
        OneShotRevA,
        RecordA,
        PatternMinusA,
        PatternPlusA,
        OneShotFwdB,
        OneShotRevB,
        RecordB,
        PatternMinusB,
        PatternPlusB
    };
    //
    //TARGET COUNT ######################################################
    //
    //Should be the same as the number of entries in Target enum
    static const int targets_count = 11;
    //
    //###################################################################

    DescreteSensor() = default;
    ~ DescreteSensor() = default;

    void initialize() {
        _state = 0;
        daisy::Mpr121I2C::Config cfg;
        _mpr.Init(cfg);

        //TARGET TO PIN MAPPING #########################################
        //
        //The order should correspond order of Target enum entries
        uint16_t mask[targets_count] = {
        #ifdef ROEY_LAYOUT
          _pin(4),                      //PlayStop
          _pin(3),                      //OneShotFwdA
          _pin(2),                      //OneShotRevA
          _together(_pin(3), _pin(2)),  //RecordA
          _pin(6),                      //PatternMinusA
          _pin(1),                      //PatternPlusA,
          _pin(5),                      //OneShotFwdB,
          _pin(0),                      //OneShotRevB,
          _together(_pin(5), _pin(0)),  //RecordB
          _pin(8),                      //PatternMinusB,
          _pin(9)                       //PatternPlusB
        #else
          _pin(7),                      //PlayStop 
          _pin(6),                      //OneShotFwdA
          _pin(4),                      //OneShotRevA
          _together(_pin(4), _pin(6)),  //RecordA
          _pin(3),                      //PatternMinusA
          _pin(2),                      //PatternPlusA,
          _pin(9),                      //OneShotFwdB,
          _pin(10),                      //OneShotRevB,
          _together(_pin(9), _pin(10)),  //RecordB
          _pin(8),                      //PatternMinusB,
          _pin(5)                       //PatternPlusB
        #endif
        };
        //
        //#################################################################

        for (auto i = 0; i < targets_count; i++) _pads[i].initialize(mask[i]);
    }

    void process() {
        auto state = _mpr.Touched();
    
    //     _buffer[_iterator] = state;
    //     _iterator++;
    //     if (_iterator < _buffer_length) return;
    //     _iterator = 0;
    //     auto _buffer2_3rds = 2 * _buffer_length / 3;
    //     for (int i = 0; i < 16; i++) {
    //         auto s = 0;
    //         for (int j = 0; j < _buffer_length; j++) {
    //             s += (_buffer[j] >> i) & 1;
    //         }
    //         if ((s >= _buffer2_3rds) && (state >> i & 1)) {
    //             state |= 1 << i;
    //         }
    //         else {
    //             state &= ~(1 << i);
    //         }
    //     }
    // #ifdef ROEY_LAYOUT
    //     state = one_or_both(3, 2, state, _state);
    //     state = one_or_both(5, 0, state, _state);
    // #else
    //     state = one_or_both(4, 6, state, _state);
    //     state = one_or_both(9, 10, state, _state);
    // #endif

        _state = state;

        for (auto& p: _pads) p.process(state);
    }
    
    void set_on_touch(std::function<void()> on_touch, Target target) {
        pad(target).on_touch = on_touch;
    }

    void set_on_release(std::function<void()> on_release, Target target) {
        pad(target).on_release = on_release;
    }

    void set_mode(DescreteSensorPad::Mode mode, Target target) {
        pad(target).mode = mode;
    }

    bool is_on(Target target) {
        return pad(target).is_on();
    }

private:
    static const int _buffer_length = 60;
    int _iterator = 0;
    uint16_t _buffer[_buffer_length];

    uint16_t _state;
    daisy::Mpr121I2C _mpr;
    DescreteSensorPad _pads[targets_count];
    
    DescreteSensorPad& pad(Target target) {
        return _pads[_target_index(target)];
    }
};
