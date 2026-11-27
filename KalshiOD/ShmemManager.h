#pragma once

#include "../PredMarketUtils/ORShmem.h"
#include "../PredMarketUtils/OrderResponse.h"

class ShmemManager {
private:
    ORShmem* or_shmem = nullptr;

    static ShmemManager* uniqueInstance;
    ShmemManager(){;}

public:
    static ShmemManager* getInstance();
    void startUp();
    void shutDown();
    void write_OR(const OrderResponse& _order_response);
};