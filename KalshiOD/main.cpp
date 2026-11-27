#include <iostream>
#include <string>

#include "KalshiWSClient.h"


int main() {
    const std::string WS_URI = "wss://user-ws.kalshi.com/trade-api/ws/v2";
    const std::string ACCESS_KEY = "5ceeecd1-df90-49a9-b8d6-4e493748c95c";
    const std::string PRIVATE_KEY_PATH = "/home/git_repos/predictionMarkets/PredMarketMD/kalshi_private_key.pem";

    try {
        KalshiWSClient client(ACCESS_KEY, PRIVATE_KEY_PATH);
        client.connect(WS_URI);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}