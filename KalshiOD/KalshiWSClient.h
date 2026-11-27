#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include "ShmemManager.h"
#include "ODProcessor.h"

using client = websocketpp::client<websocketpp::config::asio_tls_client>;
using websocketpp::connection_hdl;

// ============================================================================
// OpenSSL RSA-PSS Signer for Kalshi Authentication
// ============================================================================
class KalshiSigner {
public:
    static std::string sign_rsa_pss(const std::string& pem_path, const std::string& payload);

private:
    static std::string base64_encode(const unsigned char* input, size_t length);
};

// ============================================================================
// Kalshi WebSocket Client
// ============================================================================
class KalshiWSClient {
private:
    client ws_client_;
    connection_hdl hdl_;
    std::string access_key_;
    std::string pem_key_path_;

public:

    ShmemManager* mShmemManager;
    ODProcessor*  mODProcessor;

    KalshiWSClient(std::string access_key, std::string pem_key_path)
        : access_key_(std::move(access_key)), pem_key_path_(std::move(pem_key_path)) {
        
        ws_client_.clear_access_channels(websocketpp::log::alevel::all);
        ws_client_.init_asio();

        // Configure TLS context handler for SSL connections
        ws_client_.set_tls_init_handler([](websocketpp::connection_hdl) {
            auto ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12_client);
            return ctx;
        });

        // Set event handlers
        ws_client_.set_open_handler(bind(&KalshiWSClient::on_open, this, std::placeholders::_1));
        ws_client_.set_message_handler(bind(&KalshiWSClient::on_message, this, std::placeholders::_1, std::placeholders::_2));
        ws_client_.set_fail_handler(bind(&KalshiWSClient::on_fail, this, std::placeholders::_1));
    }

    void connect(const std::string& uri);
    void on_open(connection_hdl hdl);
    void on_message(connection_hdl, client::message_ptr msg);
    void on_fail(connection_hdl hdl);
};