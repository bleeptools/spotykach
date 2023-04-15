//
//  Engine.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 30.03.22.
//

#ifndef Engine_h
#define Engine_h

#include "ITrigger.h"
#include "ISource.h"
#include "IEnvelope.h"
#include "IGenerator.h"
#include "ILFO.h"
#include "Parameters.h"
#include "../fx/pitch.shift.h"

namespace vlly {
namespace spotykach {

struct PlaybackParameters {
    float tempo;
    float sampleRate;
};

struct RawParameters {
    float grid             = -1;
    float shift            = -1;
    float slicePosition    = -1;
    float sliceLength      = -1;
    
    float repeats          = -1;
    float retrigger        = -1;
    float jitterAmount     = -1;
    bool on                 = false;
    bool declick            = false;
    bool frozen             = false;
    bool reverse            = false;
};

class Engine {
public:
    Engine(ITrigger&, ISource&, IEnvelope&, IGenerator&, ILFO&);
    ~Engine() {};
    
    RawParameters rawParameters() { return _raw; }
    
    void initialize();
    
    void set_index(int ndx) { index = ndx; _trigger.set_index(ndx); }

    void set_is_playing(bool value, bool clean);

    void one_shot(bool reverse);

    bool isLocking() { return _trigger.locking(); };
    
    void setSlicePosition(float start);
    void setSliceLength(float slice);
    
    void set_pitch_shift(float value);

    void setShift(float shift);

    std::array<int, kGrid_Count> pattern_idexes() { return _pattern_indexes; }
    void init_pattern_indexes(std::array<int, kGrid_Count> indexes);
    int next_pattern();
    int prev_pattern();
    void set_grid(float grid);
    Grid grid() { return _grid; };
    
    void setRepeats(float repeats);
    int repeats() { return _trigger.repeats(); };
    
    void setRetrigger(float retrigger);
    
    void setJitterAmount(float value);
    void setJitterRate(float value);
    
    void setDeclick(bool declick);
    
    void setReverse(bool reverse);
    
    void preprocess(PlaybackParameters p);
    
    void setFrozen(bool frozen);
    void setAntifreeze(bool value);

    void set_on_slice(SliceCallback f);

    void step(bool engaged = true);

    void process(float in0, float in1, float* out0, float* out1, bool continual, bool reverse);

    void reset(bool hard = true);
    
    int index = -1;

private:
    ITrigger& _trigger;
    ISource& _source;
    IEnvelope& _envelope;
    IGenerator& _generator;
    ILFO& _jitterLFO;
    
    RawParameters _raw;
    
    bool _is_playing;
    float _tempo;
    
    Grid _grid;
    std::array<int, kGrid_Count> _pattern_indexes;
    int _onsets;
    int _step;
    int _shift;
    float _start;
    float _slice;
    
    bool _invalidateCrossfade;

    int set_pattern_index(int index);
    void prepare_pattern();
};
}
}

#endif /* Engine_h */
