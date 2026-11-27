#pragma once

#include "globals.h"
#include "OrderResponse.h"
#include <atomic>

struct ORShmem{
    std::atomic<int32_t> next_write_index = 0;
    OrderResponse m_queue[VAL_QUEUE_SIZE];
};