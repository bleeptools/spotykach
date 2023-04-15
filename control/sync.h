#pragma once

#include "daisy_seed.h"
#include <array>
#include "../core/globals.h"
#include "../core/core.h"

struct Beat {
    uint32_t beats = 0;
    uint32_t ticks = 0;

    float value() {
        return beats + static_cast<float>(ticks) / kTicksPerBeat;
    }

    void reset() {
        beats = 0;
        ticks = 0;
    }
};

class Sync {
public:
    Sync() = default;
    ~Sync() = default;   

    void run(vlly::spotykach::Core& core);
    void pull(daisy::DaisySeed& hw);
    float tempo();

private:
    vlly::spotykach::Core* _core;

    bool _filled = false;
    uint32_t _ptime = 0;
    std::size_t _iterator = 0;
    std::array<uint32_t, 16> _wndw; //4 ticks * 4 beats = 1 measure
    uint8_t _dev_cnt = 0; //deviations count
    uint8_t _dev_cnt_thres = 3; //deviations until reset
    uint32_t _dev_thres = 3; //min deviation to consider
    
    float _avg = 125;
    float _tempo = 120;
    Beat _beat;
    int _last_state = 1;
    
    daisy::GPIO g;

    void tick();
    void push(uint32_t interval);
    float avg();
    float tempo(float tick);
    void checkDeviation(uint32_t delta);
    void reset();
};
