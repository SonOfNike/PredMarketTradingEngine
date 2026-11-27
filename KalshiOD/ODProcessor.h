#pragma once

#include "../PredMarketUtils/OrderResponse.h"
#include "../../Utils/simdjson/simdjson.h"
#include "ShmemManager.h"
#include "../../Utils/pin_thread_functions.h"
#include <atomic>

class ODProcessor {
private:

    ShmemManager* mShmemManager;

    OrderResponse    currentOR;

    static ODProcessor* uniqueInstance;
    ODProcessor(){;}

public:

    static ODProcessor* getInstance();
    void startUp();
    void shutDown();
    void process_raw_data(const std::string& raw_json);
    void process_value(const simdjson::dom::object& obj);
    void process_fill(const simdjson::dom::object& obj);
    void process_order(const simdjson::dom::object& obj);
};