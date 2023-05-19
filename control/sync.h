#pragma once

#include "daisy_seed.h"
#include <array>
#include "../core/globals.h"
#include "../core/core.h"

namespace blptls {
namespace spotykach {

struct Beat {
    uint32_t beats = 0;
    uint32_t ticks = 0;

    float value() {
        return beats + static_cast<float>(ticks) / kPPQN;
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

    void run(Core& core);
    void advance();
    void pull(daisy::DaisySeed& hw);
    float tempo();
    void set_tempo(float normValue);
    void set_is_playing(bool is_playing);

private:
    inline uint32_t tempo_mks(const float tempo) {
        return static_cast<uint32_t>(kSecondsPerMinute * 1e6 / tempo);
    }

    Core* _core;

    bool _filled = false;
    uint32_t _ptime = 0;
    std::size_t _iterator = 0;
    std::array<uint32_t, 16> _wndw; //4 ticks * 4 beats = 1 measure
    uint8_t _dev_cnt = 0; //deviations count
    uint8_t _dev_cnt_thres = 3; //deviations until reset
    uint32_t _dev_thres = 3; //min deviation to consider

    bool _is_playing = false;

    const uint32_t kInterval = 1e6 * kBufferSize / kSampleRate;
    const uint32_t kTRtime = kPPQN * kInterval;
    uint32_t _tempo_mks = tempo_mks(_tempo);
    uint32_t _fticks = 0;
    uint32_t _nticks = 0;
    uint32_t _elapsed_ticks = 0;

    float _avg = 125;
    float _tempo = 120;
    float _raw_tempo = _tempo;
    Beat _beat;
    int _last_state = 1;
    
    daisy::GPIO g;

    void clock_in_tick();
    void push(uint32_t interval);
    float avg();
    float tempo(float tick);
    void checkDeviation(uint32_t delta);
    void reset();
};

}
}