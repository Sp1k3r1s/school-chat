#include "Server.h"

server::server(asio::io_context& io_context, short port) :
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

void server::do_accept() {
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

				std::cout << "New client connected! \n";
				s->start();
			}
			do_accept();
		});
}

void server::broadcast(const std::string& message) {
	std::lock_guard<std::mutex> lock(_sessions_mutex);
	for (auto& session : _sessions) {
		if (session) {
			session->tell_client(message);
		}
	}
}

void server::remove_session(std::shared_ptr<session> s) {
	std::lock_guard<std::mutex> lock(_sessions_mutex);
	_sessions.erase(s);
}