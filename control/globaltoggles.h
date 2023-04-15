#pragma once

#include "daisy_seed.h"
#include "layout.h"

struct GlobalToggles {
public:
    enum class Target {
        Mutex,
        Cascade,
        Split
    };

    GlobalToggles() = default;
    ~GlobalToggles() = default;

    void initialize(daisy::DaisySeed hw);

    using IsOn = bool;
    std::tuple<Target, IsOn> at(int index);

    size_t count() { return kTargetsCount; }

private:
    bool isOnAt(int index);
    daisy::Pin pin(Target t) {
        using namespace daisy;
        using namespace seed;
        switch (t) {
        #ifdef ROEY_LAYOUT
            case Target::Mutex:     return D4;
            case Target::Cascade:   return D5;
            case Target::Split:     return D3;
            default: return {};
        #else
            case Target::Mutex:     return D29;
            case Target::Cascade:   return D27;
            case Target::Split:     return D30;
            default: return {};
        #endif
        };
        
    };

    static const int kTargetsCount { 3 };
    constexpr static std::array<Target, kTargetsCount> _targets = {
        Target::Mutex,
        Target::Cascade,
        Target::Split
    };

    std::array<daisy::Switch, kTargetsCount> _switches;
};
