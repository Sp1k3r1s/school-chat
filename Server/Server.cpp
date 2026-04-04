#include <asio.hpp>
#include <asio/ssl.hpp>

#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <mutex>

class server;

class session : public std::enable_shared_from_this<session> {
	asio::ssl::stream<asio::ip::tcp::socket> _socket;
	char _data[1024];
	std::weak_ptr<server> _server;

	void do_write();

	void do_read() {
		auto self(shared_from_this());
		memset(_data, 0x0, 1024);
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

	void do_handshake() {
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

	void stop();

public:
	session(asio::ssl::stream<asio::ip::tcp::socket> socket, std::shared_ptr<server> server) :
		_socket(std::move(socket)), _server(std::move(server)) {
		memset(_data, 0x0, 1024);
	}

	void send(const std::string& message);

	void start() {
		do_handshake();
	}
};

class server : public std::enable_shared_from_this<server> {
	asio::ip::tcp::acceptor _acceptor;
	asio::ssl::context _ssl_context;

	std::set<std::shared_ptr<session>> _sessions;
	std::mutex _sessions_mutex;

	void do_accept() {
		_acceptor.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket) {
				if (!ec) {
					auto self = shared_from_this();
					auto s = std::make_shared<session>(
						asio::ssl::stream<asio::ip::tcp::socket>(std::move(socket), _ssl_context),
						self
					);

					std::lock_guard<std::mutex> lock(_sessions_mutex);
					_sessions.insert(s);

					s->start();
					std::cout << "New client connected! \n";
				}
				do_accept();
			});
	}

public:
	server(asio::io_context& io_context, short port) :
		_acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
		_ssl_context(asio::ssl::context::tls_server) {

		_ssl_context.set_options(
			asio::ssl::context::default_workarounds
			| asio::ssl::context::no_sslv2
			| asio::ssl::context::no_sslv3);

		_ssl_context.use_certificate_chain_file("server.crt");
		_ssl_context.use_private_key_file("server.key", asio::ssl::context::pem);

		do_accept();
	}

	void broadcast(const std::string& message) {
		std::lock_guard<std::mutex> lock(_sessions_mutex);
		for (auto& session : _sessions) {
			if (session) {
				session->send(message);
			}
		}
	}

	void remove_session(std::shared_ptr<session> s) {
		std::lock_guard<std::mutex> lock(_sessions_mutex);
		_sessions.erase(s);
	}
};

void session::send(const std::string& message) {
	auto self(shared_from_this());
	asio::async_write(_socket, asio::buffer(message),
		[self](const asio::error_code& e, size_t lenght) {
			if (e) self->stop();
		}
	);
}

void session::stop() {
	auto server = _server.lock();
	if (server) {
		server->remove_session(shared_from_this());
	}
}

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

int main()
{
	try {
		asio::io_context io;
		auto s = std::make_shared<server>(io, 1337);

		std::vector<std::thread> threadpool;
		unsigned int thread_count = std::thread::hardware_concurrency();
		if (thread_count == 0) thread_count = 4;

		for (unsigned int i = 0; i < thread_count; i++) {
			threadpool.emplace_back(
				[&io]() {
					io.run();
				}
			);
		}


		for (std::thread& t : threadpool) t.join();

		io.run();
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}