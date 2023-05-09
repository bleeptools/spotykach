#include "controller.h"

using namespace vlly;
using namespace spotykach;

using namespace daisy;

void Controller::initialize(DaisySeed& hw, Core& core) {
    
    init_knobs(hw);
    init_toggles(hw);
    
    hw.adc.Start();

    init_sensor(core);

    _store.initialize(hw);
    if (_store.is_updated()) {
        set_persisted(core);
    }
    else {
        auto i_a = core.engineAt(0).pattern_idexes();
        store_pattern_index_a(i_a[0], Grid::even);
        store_pattern_index_a(i_a[1], Grid::c_word);

        auto i_b = core.engineAt(1).pattern_idexes();
        store_pattern_index_b(i_b[0], Grid::even);
        store_pattern_index_b(i_b[1], Grid::c_word);
    }
}

void Controller::init_knobs(DaisySeed& hw) {
    size_t knobs_count = _knobs.size();
    
    AdcChannelConfig conf[knobs_count];
    
    size_t i;
    for (i = 0; i < knobs_count; i++) _knobs[i].initialize(conf[i], i);
        
    hw.adc.Init(conf, knobs_count);
    
    for (auto& k: _knobs) k.configure(hw);
}

void Controller::init_toggles(DaisySeed& hw) {
    _channel_toggles[0].initialize(hw, 0);
    _channel_toggles[1].initialize(hw, 1);
    _global_toggles.initialize(hw);
}

using Target = DescreteSensor::Target;
void Controller::init_sensor(Core& core) {
    _sensor.initialize();

    auto& e_a = core.engineAt(0);
    auto& e_b = core.engineAt(1);

    _sensor.set_mode(DescreteSensorPad::Mode::Toggle, Target::PlayStop);

    _sensor.set_on_touch([&e_a, this]{ 
        if (this->_rec_a) {
            e_a.clear_buffer();
        }
        else {
            this->store_pattern_index_a(e_a.prev_pattern(), e_a.grid()); 
        }
    }, Target::PatternMinusA);
    _sensor.set_on_touch([&e_a, this]{ this->store_pattern_index_a(e_a.next_pattern(), e_a.grid()); }, Target::PatternPlusA);
    _sensor.set_on_touch([&e_b, this]{ 
        if (this->_rec_b) {
            e_b.clear_buffer();
        }
        else {
            this->store_pattern_index_b(e_b.prev_pattern(), e_b.grid()); 
        }
    }, Target::PatternMinusB);
    _sensor.set_on_touch([&e_b, this]{ this->store_pattern_index_b(e_b.next_pattern(), e_b.grid()); }, Target::PatternPlusB);
}

void Controller::store_pattern_index_a(int index, Grid g) {
    switch (g) {
        case Grid::c_word: _store.set_cword_pattern_a(index); break;
        case Grid::even: _store.set_even_pattern_a(index); break;
    };
}

void Controller::store_pattern_index_b(int index, Grid g) {
    switch (g) {
        case Grid::c_word: _store.set_cword_pattern_b(index); break;
        case Grid::even: _store.set_even_pattern_b(index);  break;
    };
}

void Controller::set_persisted(Core& core) {
    auto& e_a = core.engineAt(0);
    auto& e_b = core.engineAt(1);
    e_a.init_pattern_indexes({ _store.even_pattern_a(), _store.cword_pattern_a() });
    e_b.init_pattern_indexes({ _store.even_pattern_b(), _store.cword_pattern_b() });
}

void Controller::set_parameters(Core& core, Leds& leds) {
    for (int i = 0; i < core.enginesCount(); i++) set_channel_toggles(core.engineAt(i), _channel_toggles[i], i);
    set_global_toggles(core);

    read_sensor(core, leds);

    set_knob_parameters(core);
};

using namespace vlly;
using namespace spotykach;
using KT = Knob::Target;
void Controller::set_knob_parameters(Core &s) {
    auto& a = s.engineAt(0);
    auto& b = s.engineAt(1);
    for (size_t i = 0; i < _knobs.size(); i++) {
        auto t = _knobs[i].target();
        auto v = _knobs[i].value();
        switch (t) {
            case KT::SlicePositionA:    a.setSlicePosition(v);      break;
            case KT::SliceLengthA:      a.setSliceLength(v);        break;
            case KT::RetriggerA:        a.setRetrigger(v);          break;
            case KT::JitterAmountA:     a.setJitterAmount(v);       break;
            case KT::JitterRate:        s.setJitterRate(v);         break;
            case KT::VolumeCrossfade:   s.setVolumeBalance(v);      break;
            case KT::PatternCrossfade:  s.set_pattern_balance(v);   break;
            case KT::SlicePositionB:    b.setSlicePosition(v);      break;
            case KT::SliceLengthB:      b.setSliceLength(v);        break;
            case KT::RetriggerB:        b.setRetrigger(v);          break;
            case KT::JitterAmountB:     b.setJitterAmount(v);       break;
            case KT::Pitch:             
            { 
                a.set_pitch_shift(v); 
                b.set_pitch_shift(v);
                break;
            }
            default:{}
        }
    }   
}

void Controller::set_channel_toggles(Engine& e, ChannelToggles& ct, int ei) {
    for (size_t i = 0; i < ct.count(); i++) {
        auto toggle = ct.at(i);
        auto target = std::get<0>(toggle);
        auto isOn = std::get<1>(toggle);
        auto holding_fwd = ei == 0 ? _holding_fwd_a : _holding_fwd_b;
        auto holding_rev = ei == 0 ? _holding_rev_a : _holding_rev_b;
        using Target = ChannelToggles::Target;
        switch (target) {
            case Target::Grid:      e.set_grid(isOn ? 1 : 0); break;
            case Target::Reverse:   e.setReverse((isOn && !holding_fwd) || holding_rev); break;
            default: {}
        }
    }
}

void Controller::set_global_toggles(Core& s) {
    using Target = GlobalToggles::Target;
    auto cnt = _global_toggles.count();
    for (size_t i = 0; i < cnt; i++) {
        auto toggle = _global_toggles.at(i);
        auto target = std::get<0>(toggle);
        auto isOn = std::get<1>(toggle);
        switch (target) {
            case Target::Mutex: s.setMutex(isOn); break;
            case Target::Cascade: s.setCascade(isOn); break;
            case Target::Split: s.setSplit(isOn); break;
        }
    }
}

void Controller::read_sensor(Core& core, Leds& leds) {
    _sensor.process();

    auto& e_a = core.engineAt(0);
    auto& e_b = core.engineAt(1);

    _rec_a = _sensor.is_on(Target::RecordA);
    _rec_b = _sensor.is_on(Target::RecordB);

    e_a.setFrozen(!_rec_a);
    e_b.setFrozen(!_rec_b);

    _holding_fwd_a = _sensor.is_on(Target::OneShotFwdA);
    _holding_fwd_b = _sensor.is_on(Target::OneShotFwdB);
    _holding_rev_a = !_rec_a && _sensor.is_on(Target::OneShotRevA);
    _holding_rev_b = !_rec_b && _sensor.is_on(Target::OneShotRevB);
    

    auto is_playing_toggled = _sensor.is_on(Target::PlayStop);
    auto holding_a = _holding_fwd_a || _holding_rev_a;
    auto holding_b = _holding_fwd_b || _holding_rev_b;

    auto reset = is_playing_toggled && !(_is_playing_toggled || holding_a || holding_b);
    _is_playing_toggled = is_playing_toggled;

    e_a.set_is_playing(is_playing_toggled, reset);
    e_b.set_is_playing(is_playing_toggled, reset);

    PlaybackControls pc;
    pc.ctns_a = !(_rec_a && is_playing_toggled) && (_holding_fwd_a || _holding_rev_a);
    pc.ctns_b = !(_rec_b && is_playing_toggled) && (_holding_fwd_b || _holding_rev_b);
    pc.rev_a = _holding_rev_a;
    pc.rev_b = _holding_rev_b;
    core.set_playback_controls(pc);

    leds.set_led_a_on(holding_a && !is_playing_toggled);
    leds.set_led_b_on(holding_b && !is_playing_toggled);
    leds.set_rec_on(_rec_a || _rec_b);
}
