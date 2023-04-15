#pragma once

namespace vlly {
namespace spotykach {
    struct Channel {
        int index;

        constexpr Channel(const int index) : index { index } {}

        constexpr bool operator==(const Channel &rhs) const {
            return (rhs.index == index);
        }

        constexpr bool operator!=(const Channel &rhs) const { return !operator==(rhs); }
    };

    constexpr Channel ChannelOne = Channel(0);
    constexpr Channel ChannelTwo = Channel(1);
}
}
