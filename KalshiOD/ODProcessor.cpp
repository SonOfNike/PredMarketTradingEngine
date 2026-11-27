#include "ODProcessor.h"
#include "../PredMarketUtils/Time_functions.h"
#include "../PredMarketUtils/math_functions.h"
#include "glog/logging.h"
#include <string>
#include <string_view>
#include <ctime>
#include <cstdint>
#include <iostream>

ODProcessor* ODProcessor::uniqueInstance = nullptr;

ODProcessor* ODProcessor::getInstance(){
    if(uniqueInstance == nullptr){
        uniqueInstance = new ODProcessor();
    }
    return uniqueInstance;
}

void ODProcessor::startUp(){
    mShmemManager = ShmemManager::getInstance();
}

void ODProcessor::shutDown(){
    
}

void ODProcessor::process_fill(const simdjson::dom::object& _obj){

    std::string_view current_sym;
    double current_val = 0;
    Timestamp current_time = 0;

    auto error = _obj["val"].get(current_val)
                | _obj["T"].get(current_sym)
                | _obj["t"].get(current_time);

    if(error)
        return;

    // currentOR.m_index = string_to_indices(current_sym);
    // currentOR.m_price = roundToNearestCent(Price(current_val * DOLLAR));

    //     //Timestamp conversion
    // currentOR.m_timestamp = current_time * MILLI_SECONDS;

    mShmemManager->write_OR(currentOR);
    
}

void ODProcessor::process_order(const simdjson::dom::object& _obj){

    std::string_view current_sym;
    double current_val = 0;
    Timestamp current_time = 0;

    auto error = _obj["val"].get(current_val)
                | _obj["T"].get(current_sym)
                | _obj["t"].get(current_time);

    if(error)
        return;

    // currentOR.m_index = string_to_indices(current_sym);
    // currentOR.m_price = roundToNearestCent(Price(current_val * DOLLAR));

    //     //Timestamp conversion
    // currentOR.m_timestamp = current_time * MILLI_SECONDS;

    mShmemManager->write_OR(currentOR);
    
}

void ODProcessor::process_raw_data(const std::string& raw_json){
 
    simdjson::dom::parser parser;

    auto result = parser.parse(raw_json);

    if(!result.error()){
        for(simdjson::dom::object obj : result){
            simdjson::dom::element type_field;
            if (obj["type"].get(type_field) == simdjson::SUCCESS){

                std::string_view type = type_field.get_string();

                switch(type[0]){
                    case 'u': 
                        process_order(obj);
                        continue;
                    case 'f': 
                        process_fill(obj);
                        continue;
                }
            }
        }
    }
}