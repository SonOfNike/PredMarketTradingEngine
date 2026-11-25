#pragma once

#include "../PredMarketUtils/ValShmem.h"
#include "../PredMarketUtils/ValUpdate.h"

class ShmemManager {
private:
    ValShmem* val_shmem = nullptr;

    static ShmemManager* uniqueInstance;
    ShmemManager(){;}

public:
    static ShmemManager* getInstance();
    void startUp();
    void shutDown();
    void write_Val(const ValUpdate& _val_update);
};