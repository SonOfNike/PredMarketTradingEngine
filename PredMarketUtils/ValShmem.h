#pragma once

#include "globals.h"
#include "ValUpdate.h"
#include <atomic>

struct ValShmem{
    std::atomic<int32_t> next_write_index = 0;
    ValUpdate m_queue[VAL_QUEUE_SIZE];
};