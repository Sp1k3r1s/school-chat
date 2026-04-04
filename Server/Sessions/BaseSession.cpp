#include "BaseSession.h"

#include <cstring>
#include <iostream>
#include <utility>

#include "../Server.h"

void session::do_write() {
    auto self(shared_from_this());
    _socket.async_write_some(asio::buffer(_data, 1024),
        [self](std::error_code ec, std::size_t length) {
            if (!ec) {
                auto serverPtr = self->_server.lock();
                if (serverPtr) {
                    serverPtr->broadcast(std::string(self->_data, length));
                }
                self->do_read();
            }
            else {
                self->stop();
            }
        });
}

void session::do_read() {
    auto self(shared_from_this());
    std::memset(_data, 0x0, 1024);
    _socket.async_read_some(asio::buffer(_data, 1024),
        [self](std::error_code ec, std::size_t length) {
            if (!ec) {
                self->do_write();
            }
            else {
                self->stop();
            }
        });
}

void session::do_handshake() {
    auto self(shared_from_this());
    _socket.async_handshake(asio::ssl::stream_base::server,
        [self](std::error_code ec) {
            if (!ec) {
                std::cout << "Client handshare success!\n";
                self->do_read();
            }
            else {
                self->stop();
            }
        });
}

void session::stop() {
    auto server = _server.lock();
    if (server) {
        server->remove_session(shared_from_this());
    }
}

session::session(asio::ssl::stream<asio::ip::tcp::socket> socket, std::shared_ptr<server> server)
    : _socket(std::move(socket)), _server(std::move(server)) {
    std::memset(_data, 0x0, 1024);
}

void session::tell_client(const std::string& message) {
    auto self(shared_from_this());
    asio::async_write(_socket, asio::buffer(message),
        [self](const asio::error_code& e, size_t lenght) {
            if (e) self->stop();
        }
    );
}

void session::start() {
    do_handshake();
}
