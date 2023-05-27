#pragma once

#include "daisy_seed.h"
#include <array>
#include "../core/globals.h"
#include "clockable.h"

namespace blptls {
namespace spotykach {

class Sync {
public:
    Sync() = default;
    ~Sync() = default;   

    void run(Clockable& core);
    void tick();
    void pull(daisy::DaisySeed& hw);
    
    float tempo() { return 60000000.f / _tempo_mks; }
    void set_tempo(float normValue);

    void set_is_playing(bool is_playing);

private:
    void external_clock_tick();
    void sync();
    void reset();
    uint32_t tempo_mks(const float tempo) {
        return static_cast<uint32_t>(kSecondsPerMinute * 1e6 / tempo);
    }

    Clockable* _clockable;
    daisy::GPIO g;

    bool _is_playing = false;
    bool _is_about_to_play = false;

    const uint32_t kInterval = 1e6 * kBufferSize / kSampleRate;
    const uint32_t kTRtime = kPPQN * kInterval;
    const uint32_t kTicksPerClock = kPPQN / 4;
    uint32_t _tempo_delta_mks = 0;
    uint32_t _ticks = 0;
    uint32_t _fticks = 0;
    uint32_t _ticks_at_last_clock = 0;
    uint32_t _tempo_ticks = 0;
    bool _hold = false;
    bool _resync = false;

    float _tempo = 120;
    float _raw_tempo = _tempo;
    uint32_t _tempo_mks = 500000;

    int _last_state = 1;
};

}
}