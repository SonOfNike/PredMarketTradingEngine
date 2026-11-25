#include "MDProcessor.h"
#include "../PredMarketUtils/Time_functions.h"
#include "../PredMarketUtils/math_functions.h"
#include "glog/logging.h"
#include <string>
#include <string_view>
#include <ctime>
#include <cstdint>
#include <iostream>

MDProcessor* MDProcessor::uniqueInstance = nullptr;

MDProcessor* MDProcessor::getInstance(){
    if(uniqueInstance == nullptr){
        uniqueInstance = new MDProcessor();
    }
    return uniqueInstance;
}

void MDProcessor::startUp(){
    mShmemManager = ShmemManager::getInstance();
}

void MDProcessor::shutDown(){
    
}

void MDProcessor::process_value(const simdjson::dom::object& _obj){

    std::string_view current_sym;
    double current_val = 0;
    Timestamp current_time = 0;

    auto error = _obj["val"].get(current_val)
                | _obj["T"].get(current_sym)
                | _obj["t"].get(current_time);

    if(error)
        return;

    currentVal.m_index = string_to_indices(current_sym);
    currentVal.m_price = roundToNearestCent(Price(current_val * DOLLAR));

        //Timestamp conversion
    currentVal.m_timestamp = current_time * MILLI_SECONDS;

    mShmemManager->write_Val(currentVal);
    
}

void MDProcessor::process_raw_data(const std::string& raw_json){
 
    simdjson::dom::parser parser;

    auto result = parser.parse(raw_json);

    if(!result.error()){
        for(simdjson::dom::object obj : result){
            simdjson::dom::element ev_field;
            if (obj["ev"].get(ev_field) == simdjson::SUCCESS){

                std::string_view type = ev_field.get_string();

                switch(type[0]){
                    case 'V': 
                        process_value(obj);
                        continue;
                }
            }
        }
    }
}