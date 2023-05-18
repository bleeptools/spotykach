#pragma once

#include <array>
#include <math.h>

namespace blptls {
namespace spotykach {
    static const uint32_t kEnginesCount     { 2 };

    static const uint32_t kSlicesCount      { 3 };
    static const uint32_t kSliceMaxSeconds  { 2 };
    static const uint32_t kSourceMaxSeconds { 10 };

    static const uint32_t kChannelsCount    { 2 };
    static const uint32_t kSampleRate       { 48000 };

    static const float kSecondsPerMinute    { 60.0 };

    static constexpr float logVolume(float value) {
        return (pow(10.0, value) - 1) / 9.0;
    }

    static const int kDirections_Count = 2;
    enum class Direction {
        forward,
        reverse
    };

    static const int kGrid_Count = 2;
    enum class Grid {
        even,
        c_word
    };

    static const uint32_t kBeatsPerMeasure  { 4 };
    static const uint32_t kTicksPerBeat     { 96 };

    using Step96ppgn = int;
    static constexpr std::array<Step96ppgn, 8> EvenSteps {{
        384, //1
        288, //1 / 2.
        192,  //1 / 2
        144,  //1 / 4.
        96,  //1 / 4
        72,  //1 / 8.
        48,  //1 / 8
        24,  //1 / 16
    }};

    using CWordOnsets = int;
    static constexpr std::array<CWordOnsets, 9> CWords {{
        3,
        5,
        6,
        7,
        9,
        10,
        11,
        12,
        13
    }};

    static constexpr int EvenStepsCount = EvenSteps.size();
    static constexpr int CWordsCount = CWords.size();
}
}
