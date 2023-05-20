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
    
    float tempo() { return 60000000.f / _tempo_mks; }
    void set_tempo(float normValue);

    void set_is_playing(bool is_playing);

private:
    void clock_in_tick();
    void sync();
    void reset();
    uint32_t tempo_mks(const float tempo) {
        return static_cast<uint32_t>(kSecondsPerMinute * 1e6 / tempo);
    }

    Core* _core;
    daisy::GPIO g;

    bool _is_playing = false;

    const uint32_t kInterval = 1e6 * kBufferSize / kSampleRate;
    const uint32_t kTRtime = kPPQN * kInterval;
    const uint32_t kTicksPerClock = kPPQN / 4;
    uint32_t _tempo_mks = tempo_mks(_tempo);
    uint32_t _tempo_delta_mks = 0;
    uint32_t _ticks = 0;
    uint32_t _fticks = 0;
    uint32_t _ticks_at_last_clock = 0;
    uint32_t _tempo_ticks = 0;

    bool _hold = false;
    bool _resync = false;

    Beat _beat;
    float _tempo = 120;
    float _raw_tempo = _tempo;
    int _last_state = 1;
};

}
}