cmake_minimum_required(VERSION 3.10.0)
project(PredMarketMD VERSION 0.1.0 LANGUAGES C CXX)

find_package(OpenSSL REQUIRED)
find_package(glog REQUIRED PATHS ../../glog/build/)

# Path to the websocketpp library
set(WEBSOCKETPP_PATH /home/git_repos/websocketpp)

add_executable(PredMarketMD main.cpp
                         KalshiWSClient.cpp
                         NovigWSClient.cpp
                         PolyWSClient.cpp
                         ../../Utils/simdjson/simdjson.cpp)

target_link_libraries(PredMarketMD PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(PredMarketMD PRIVATE glog::glog)
target_include_directories(PredMarketMD PUBLIC
        ${WEBSOCKETPP_PATH}
    )

include(CTest)
enable_testing()

set_property(TARGET PredMarketMD PROPERTY CXX_STANDARD 17)