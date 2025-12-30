#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace boost::asio;
using ip::tcp;

int main() {
    try {
        io_context io_ctx;

        tcp::socket socket(io_ctx);
        socket.connect(tcp::endpoint(ip::address::from_string("127.0.0.1"), 12345));  // Connect to localhost:12345

        std::cout << "Connected to server!" << std::endl;

        std::string message;
        while (std::getline(std::cin, message)) {  // Read from console
            if (message == "quit") break;

            write(socket, buffer(message + "\n"));  // Send with newline

            std::array<char, 1024> buf;
            size_t len = socket.read_some(buffer(buf));
            std::string reply(buf.data(), len);
            std::cout << "Echo: " << reply << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}