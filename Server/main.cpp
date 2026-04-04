#include "Server.h"

#include <thread>
#include <vector>

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
