#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>

#include <memory>
#include <string>

class server;

class session : public std::enable_shared_from_this<session> {
    asio::ssl::stream<asio::ip::tcp::socket> _socket;
    char _data[1024];
    std::weak_ptr<server> _server;

    void do_write();
    void do_read();
    void do_handshake();
    void stop();

public:
    session(asio::ssl::stream<asio::ip::tcp::socket> socket, std::shared_ptr<server> server);

    void tell_client(const std::string& message);
    void start();
};
