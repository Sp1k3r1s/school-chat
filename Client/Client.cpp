#include <asio.hpp>
#include <asio/ssl.hpp>

#include <array>
#include <iostream>
#include <memory>
#include <string_view>

class client : public std::enable_shared_from_this<client>
{
    asio::io_context& _io_context;
    asio::ssl::stream<asio::ip::tcp::socket> _socket;
    asio::ip::tcp::resolver _resolver;

    char _recv_buffer[1024];
    std::string _write_buffer;

    void do_read()
    {
        auto self(shared_from_this());
        _socket.async_read_some(asio::buffer(_recv_buffer),
            [self](const asio::error_code& e, std::size_t length) {
                if (!e) {
                    std::cout << "Received: "
                              << std::string_view(self->_recv_buffer, length)
                              << std::endl;
                    self->do_write();
                }
                else {
                    std::cerr << "Failed to receive: " << e.message() << std::endl;
                }
            });
    }

    void do_write()
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
                    self->do_read();
                }
                else {
                    std::cerr << "Failed to write message: " << e.message() << std::endl;
                }
            });
    }

    void do_handshake()
    {
        auto self(shared_from_this());
        _socket.async_handshake(asio::ssl::stream_base::client,
            [self](const asio::error_code& e) {
                if (!e) {
                    self->do_write();
                }
                else {
                    std::cerr << "Failed to do handshake: " << e.message() << std::endl;
                }
            });
    }

    void connect(asio::ip::tcp::resolver::results_type endpoints)
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

public:
    client(asio::io_context& io, asio::ssl::context& ctx)
        : _io_context(io), _socket(io, ctx), _resolver(io) {}

    void start(const std::string& host, const std::string& port)
    {
        auto self(shared_from_this());
        _resolver.async_resolve(host, port,
            [self](const asio::error_code& e, asio::ip::tcp::resolver::results_type res) {
                if (!e) {
                    self->connect(std::move(res));
                }
                else {
                    std::cerr << "Resolver error: " << e.message() << std::endl;
                }
            });
    }
};

int main()
{
    try {
        asio::io_context io_context;
        asio::ssl::context ssl_context(asio::ssl::context::tls_client);

        ssl_context.set_verify_mode(asio::ssl::verify_none);

        auto c = std::make_shared<client>(io_context, ssl_context);
        c->start("127.0.0.1", "1337");
        io_context.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
