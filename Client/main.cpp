#include "Client.h"

#include <iostream>

int main()
{
    try {
        asio::io_context io_context;
        asio::ssl::context ssl_context(asio::ssl::context::tls_client);

        ssl_context.set_verify_mode(asio::ssl::verify_none);

        auto c = std::make_shared<client>(io_context, ssl_context);
        //c->start("127.0.0.1", "1337");

        asio::ip::tcp::resolver resolve(io_context);
        auto endpoints = resolve.resolve("127.0.0.1", "1337");
        asio::connect(c->_socket.lowest_layer(), endpoints);
        c->do_handshake();

        std::thread io_thread([&io_context]() { io_context.run(); });

        while (true) {
            c->do_write();
        }

        io_context.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
