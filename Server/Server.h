#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <mutex>

#include "Sessions/BaseSession.h"

class server : public std::enable_shared_from_this<server> {
	asio::ip::tcp::acceptor _acceptor;
	asio::ssl::context _ssl_context;

	std::set<std::shared_ptr<session>> _sessions;
	std::mutex _sessions_mutex;

	void do_accept();

public:
	server(asio::io_context& io_context, short port);

	void broadcast(const std::string& message);
	void remove_session(std::shared_ptr<session> s);
};
