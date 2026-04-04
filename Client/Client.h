#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <memory>


class client : public std::enable_shared_from_this<client>
{
public:
    asio::io_context& _io_context;
    asio::ip::tcp::resolver _resolver;
    asio::ssl::stream<asio::ip::tcp::socket> _socket;

    char _recv_buffer[1024];
    std::string _write_buffer;

    void do_read();
    void do_write();
    void do_handshake();
    void connect(asio::ip::tcp::resolver::results_type endpoints);


    client(asio::io_context& io, asio::ssl::context& ctx);
};