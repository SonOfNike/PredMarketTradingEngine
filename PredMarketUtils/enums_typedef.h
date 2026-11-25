#pragma once

#include <string_view>
#include <optional>
#include <array>

enum class Indices {SPX, NDX, VIX, VIX1D, VXN, None};

constexpr std::array AllIndices = {
    Indices::SPX,
    Indices::NDX,
    Indices::VIX,
    Indices::VIX1D,
    Indices::VXN
};

constexpr std::string_view to_string(Indices c) {
    switch (c) {
        case Indices::SPX:   return "SPX";
        case Indices::NDX:   return "NDX";
        case Indices::VIX:   return "VIX";
        case Indices::VIX1D:   return "VIX1D";
        case Indices::VXN:   return "VXN";
    }
    return "Unknown";
}

constexpr Indices string_to_indices(std::string_view str) {
    if (str == "I:SPX")   return Indices::SPX;
    if (str == "I:NDX") return Indices::NDX;
    if (str == "I:VIX")  return Indices::VIX;
    if (str == "I:VIX1D")  return Indices::VIX1D;
    if (str == "I:VXN")  return Indices::VXN;
    return Indices::None;
}

enum class side {BUY, SELL, NONE};

typedef int64_t Price;
typedef int32_t Shares;
typedef uint64_t Timestamp;
typedef int64_t MyOrderId;
typedef int16_t SymbolId;