#include <asio.hpp>
#include <asio/ssl.hpp>

#include <iostream>
#include <memory>

class session : public std::enable_shared_from_this<session> {
	asio::ssl::stream<asio::ip::tcp::socket> _socket;
	char _data[1024];

	void do_read() {
		auto self(shared_from_this());
		memset(_data, 0x0, 1024);
		_socket.async_read_some(asio::buffer(_data, 1024),
			[this, self](std::error_code ec, std::size_t length) {
				if (!ec) {
					do_write();
				}
			});
	}

	void do_write() {
		auto self(shared_from_this());
		_socket.async_write_some(asio::buffer(_data, 1024),
			[this, self](std::error_code ec, std::size_t length) {
				if (!ec) {
					do_read();
				}
			});
	}

	void do_handshake() {
		auto self(shared_from_this());
		_socket.async_handshake(asio::ssl::stream_base::server,
			[this, self](std::error_code ec) {
				if (!ec) {
					std::cout << "Client handshare success!\n";
					do_read();
				}
			});
	}
	
public:
	session(asio::ssl::stream<asio::ip::tcp::socket> socket) : _socket(std::move(socket)) {}

	void start() {
		do_handshake();
	}
};

class server {
	asio::ip::tcp::acceptor _acceptor;
	asio::ssl::context _ssl_context;

	void do_accept() {
		_acceptor.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket) {
				if (!ec) {
					std::cout << "New client connected! \n";
					std::make_shared<session>(
						asio::ssl::stream<asio::ip::tcp::socket>(std::move(socket), _ssl_context)
					)->start();
				}
				do_accept();
			});
	}

public:
	server(asio::io_context& io_context, short port) : 
		_acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
		_ssl_context(asio::ssl::context::sslv23) {

		_ssl_context.set_options(
			asio::ssl::context::default_workarounds
			| asio::ssl::context::no_sslv2
			| asio::ssl::context::no_sslv3);

		_ssl_context.use_certificate_chain_file("server.crt");
		_ssl_context.use_private_key_file("server.key", asio::ssl::context::pem);

		do_accept();
	}
};

int main()
{
	try {
		asio::io_context io;
		server s(io, 1337);
		io.run();
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}