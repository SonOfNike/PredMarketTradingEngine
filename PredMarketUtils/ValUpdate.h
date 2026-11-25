#pragma once

#include "enums_typedef.h"

struct alignas(64) ValUpdate{
    Price m_price = 0;
    Timestamp m_timestamp = 0;
    Indices m_index = Indices::None;
};