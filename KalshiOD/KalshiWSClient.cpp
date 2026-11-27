#include "KalshiWSClient.h"


std::string KalshiSigner::sign_rsa_pss(const std::string& pem_path, const std::string& payload) {
    BIO* bio = BIO_new_file(pem_path.c_str(), "r");
    if (!bio) {
        std::cerr << "[SIGN ERROR] Could not open PEM key file at path: " << pem_path << "\n";
        return "";
    }

    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (!pkey) {
        std::cerr << "[SIGN ERROR] Failed to parse private key from PEM file.\n";
        return "";
    }

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_PKEY_CTX* pkey_ctx = nullptr;
    std::string signature_base64;

    if (EVP_DigestSignInit(ctx, &pkey_ctx, EVP_sha256(), nullptr, pkey) <= 0) {
        std::cerr << "[SIGN ERROR] EVP_DigestSignInit failed.\n";
    } else {
        // Set PSS Padding, Salt Length = 32 (digest length), and MGF1 = SHA256
        EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PKCS1_PSS_PADDING);
        EVP_PKEY_CTX_set_rsa_pss_saltlen(pkey_ctx, RSA_PSS_SALTLEN_DIGEST);
        EVP_PKEY_CTX_set_rsa_mgf1_md(pkey_ctx, EVP_sha256());

        if (EVP_DigestSignUpdate(ctx, payload.c_str(), payload.size()) <= 0) {
            std::cerr << "[SIGN ERROR] EVP_DigestSignUpdate failed.\n";
        } else {
            size_t sig_len = 0;
            EVP_DigestSignFinal(ctx, nullptr, &sig_len); // Get signature size
            
            std::vector<unsigned char> sig(sig_len);
            if (EVP_DigestSignFinal(ctx, sig.data(), &sig_len) <= 0) {
                std::cerr << "[SIGN ERROR] EVP_DigestSignFinal failed.\n";
            } else {
                signature_base64 = base64_encode(sig.data(), sig_len);
            }
        }
    }

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    if (signature_base64.empty()) {
        std::cerr << "[SIGN ERROR] Signature generation resulted in empty string.\n";
    }

    return signature_base64;
}

std::string KalshiSigner::base64_encode(const unsigned char* input, size_t length) {
    BIO* bmem = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    // NO_NL is mandatory so OpenSSL doesn't append '\n' characters inside the signature string
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO* push = BIO_push(b64, bmem);

    BIO_write(push, input, static_cast<int>(length));
    BIO_flush(push);

    BUF_MEM* bptr;
    BIO_get_mem_ptr(push, &bptr);

    std::string result(bptr->data, bptr->length);
    BIO_free_all(push);
    return result;
}

void KalshiWSClient::connect(const std::string& uri) {
    websocketpp::lib::error_code ec;
    client::connection_ptr conn = ws_client_.get_connection(uri, ec);
    if (ec) {
        std::cerr << "Initialization error: " << ec.message() << std::endl;
        return;
    }

    // --- Kalshi RSA-PSS Handshake Signing ---
    // Payload string: timestamp_ms + "GET" + "/trade-api/ws/v2"
    auto now = std::chrono::system_clock::now();
    auto ts_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now.time_since_epoch()).count();
    std::string ts_str = std::to_string(ts_ms);

    std::string path = "/trade-api/ws/v2";
    std::string sign_payload = ts_str + "GET" + path;
    std::string signature = KalshiSigner::sign_rsa_pss(pem_key_path_, sign_payload);

    // Append required Kalshi HTTP headers to the upgrade request
    conn->append_header("KALSHI-ACCESS-KEY", access_key_);
    conn->append_header("KALSHI-ACCESS-SIGNATURE", signature);
    conn->append_header("KALSHI-ACCESS-TIMESTAMP", ts_str);

    ws_client_.connect(conn);
    ws_client_.run();
}

void KalshiWSClient::on_open(connection_hdl hdl) {
    hdl_ = hdl;
    std::cout << "[WS] Connected and authenticated successfully!\n";

    // Subscribe to a market orderbook feed
    std::string subscribe_msg = R"({
        "id": 2,
        "cmd": "subscribe",
        "params": {
            "channels": ["user_orders", "fill"]
        }
    })";

    websocketpp::lib::error_code ec;
    ws_client_.send(hdl_, subscribe_msg, websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cerr << "[WS] Subscription failed: " << ec.message() << std::endl;
    }
}

void KalshiWSClient::on_message(connection_hdl, client::message_ptr msg) {
    std::cout << "[RECV] " << msg->get_payload() << std::endl;

    mODProcessor->process_raw_data(msg->get_payload());
}

void KalshiWSClient::on_fail(connection_hdl hdl) {
    client::connection_ptr conn = ws_client_.get_con_from_hdl(hdl);
    std::cerr << "[WS] Handshake failed: " << conn->get_ec().message() << std::endl;

    // Read response code (e.g., 401 Unauthorized, 403 Forbidden)
    std::cerr << "HTTP Status: " << conn->get_response_code() << "\n";
    std::cerr << "HTTP Reason: " << conn->get_response_msg() << "\n";

    // Access the response object body
    std::cerr << "HTTP Response Body: " << conn->get_response().get_body() << "\n";
}