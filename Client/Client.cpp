#include "Client.h"

#include <iostream>

client::client(asio::io_context& io, asio::ssl::context& ctx)
    : _io_context(io), _socket(io, ctx), _resolver(io) { }

void client::do_read()
{
    auto self(shared_from_this());
    _socket.async_read_some(asio::buffer(_recv_buffer),
        [self](const asio::error_code& e, std::size_t length) {
            if (!e) {
                std::cout << "Received: "
                    << std::string_view(self->_recv_buffer, length)
                    << std::endl;
                self->do_read();
            }
            else {
                std::cerr << "Failed to receive: " << e.message() << std::endl;
            }
        });
}

void client::do_write()
{
    std::cout << "Enter your message -> ";
    if (!std::getline(std::cin >> std::ws, _write_buffer)) {
        std::cerr << "Failed to read input." << std::endl;
        return;
    }

    auto self(shared_from_this());
    asio::async_write(_socket, asio::buffer(_write_buffer),
        [self](const asio::error_code& e, std::size_t length) {
            if (!e) {
                std::cout << "Sent " << length << " bytes." << std::endl;
                //self->do_read();
                self->do_read();
            }
            else {
                std::cerr << "Failed to write message: " << e.message() << std::endl;
            }
        });
}

void client::do_handshake()
{
    auto self(shared_from_this());
    _socket.async_handshake(asio::ssl::stream_base::client,
        [self](const asio::error_code& e) {
            if (!e) {
                self->do_read();
            }
            else {
                std::cerr << "Failed to do handshake: " << e.message() << std::endl;
            }
        });
}

void client::connect(asio::ip::tcp::resolver::results_type endpoints)
{
    auto self(shared_from_this());
    asio::async_connect(_socket.lowest_layer(), endpoints,
        [self](const asio::error_code& e, const asio::ip::tcp::endpoint&) {
            if (!e) {
                self->do_handshake();
            }
            else {
                std::cerr << "Failed to connect: " << e.message() << std::endl;
            }
        });
}