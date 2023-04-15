//
//  Core.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 22.05.22.
//

#pragma once

#include "Engine.h"
#include "Parameters.h"
#include "globals.h"
#include <array>
#include <vector>
#include <memory>


namespace vlly {
namespace spotykach {

struct PlaybackControls {
    bool ctns_a;
    bool ctns_b;
    bool rev_a;
    bool rev_b;
};

class Core {
public:
    Core();
    ~Core() {};
    
    Engine& engineAt(int index) const;
    long enginesCount() const;

    void step();

    void setMutex(bool mutex);
    
    void setSplit(bool value);
    
    void setVolumeBalance(float value);

    void set_pattern_balance(float value);

    void setCascade(bool value);
    
    void setJitterRate(float normVal);

    void set_playback_controls(PlaybackControls c);

    void initialize() const;
    void preprocess(PlaybackParameters p) const;
    void process(const float* const* inBuf, float** outBuf, int numFrames) const;
    
private:
    std::array<std::shared_ptr<Engine>, kEnginesCount> _engines;
    
    float _vol[kEnginesCount];
    float pattern_balance_;
    bool _split;
    bool _mutex;
    bool _cascade;
    PlaybackControls _p_ctrls;
    std::vector<std::shared_ptr<void>> _releasePool;
};
}
}
