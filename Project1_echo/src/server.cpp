#include <boost/asio.hpp>  // Core Asio header
#include <iostream>        // For output
#include <string>          // For data handling

using namespace boost::asio;  // Namespace alias for brevity
using ip::tcp;                // Shortcut for tcp namespace

int main() {
    try {
        io_context io_ctx;  // The I/O context manages operations

        tcp::acceptor acceptor(io_ctx, tcp::endpoint(tcp::v4(), 12345));  // Listen on port 12345

        std::cout << "Server listening on port 12345..." << std::endl;

        while (true) {  // Loop to accept multiple clients (simple, not production-ready)
            tcp::socket socket(io_ctx);  // New socket for each client
            acceptor.accept(socket);     // Block until client connects

            std::cout << "Client connected!" << std::endl;

            std::array<char, 1024> buf;  // Fixed-size buffer for data
            while (true) {
                size_t len = socket.read_some(buffer(buf));  // Read data (sync, blocks)
                if (len == 0) break;  // Client disconnected

                std::string data(buf.data(), len);  // Convert to string
                std::cout << "Received: " << data << std::endl;

                write(socket, buffer(data));  // Echo back
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}