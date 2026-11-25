#include <iostream>
#include <string>

#include "KalshiWSClient.h"
#include "NovigWSClient.h"
#include "PolyWSClient.h"


int main() {
    const std::string WS_URI = "wss://external-api-ws.kalshi.com/trade-api/ws/v2";
    const std::string ACCESS_KEY = "5ceeecd1-df90-49a9-b8d6-4e493748c95c";
    const std::string PRIVATE_KEY_PATH = "/home/git_repos/predictionMarkets/PredMarketMD/kalshi_private_key.pem";

    // const std::string POLY_WS_URI = "wss://api.polymarket.us/v1/ws/markets";
    // const std::string POLY_ACCESS_KEY = "f8c4d773-e4d6-4933-a373-32470f499ce1";
    // const std::string POLY_SECRET_KEY = "ICLV/j0wq0m+bACgpl+VZ2a+XIXffVeticXwZcnglipuXcDclG4stBOUDuZM5RxSPCHapV6HYoVOhP6002/jZw==";

    try {
        KalshiWSClient client(ACCESS_KEY, PRIVATE_KEY_PATH);
        client.connect(WS_URI);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    // try {
    //     PolyWSClient client(POLY_ACCESS_KEY, POLY_SECRET_KEY);
    //     client.connect(POLY_WS_URI);
    // } catch (const std::exception& e) {
    //     std::cerr << "Exception: " << e.what() << std::endl;
    // }

    return 0;
}