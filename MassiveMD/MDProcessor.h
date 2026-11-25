#pragma once

#include "../PredMarketUtils/ValUpdate.h"
#include "../../Utils/simdjson/simdjson.h"
#include "ShmemManager.h"
#include "../../Utils/pin_thread_functions.h"
#include <atomic>

class MDProcessor {
private:

    ShmemManager* mShmemManager;

    ValUpdate    currentVal;

    static MDProcessor* uniqueInstance;
    MDProcessor(){;}

public:

    static MDProcessor* getInstance();
    void startUp();
    void shutDown();
    void process_raw_data(const std::string& raw_json);
    void process_value(const simdjson::dom::object& obj);
};