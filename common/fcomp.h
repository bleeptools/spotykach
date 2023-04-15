
#pragma once

inline static bool fcomp(float lhs, float rhs, int precision = 2) {
    auto digits = precision * 10;
    auto lhs_int = static_cast<int>(roundf(lhs * digits));
    auto rhs_int = static_cast<int>(roundf(rhs * digits));
    return lhs_int == rhs_int;
}
