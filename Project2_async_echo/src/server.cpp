#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <array>

using boost::asio::ip::tcp;
using namespace boost::asio;

class session
      : public std::enable_shared_from_this<session>
{
  public:
    session(any_io_executor exec)
          : socket_(exec),
            strand_(make_strand(exec))
    {
        ++active_connections;
        std::cout << "Active connections: "
                  << active_connections.load() << "\n";
    }

    ~session()
    {
        --active_connections;
        std::cout << "Active connections: "
                  << active_connections.load() << "\n";
    }

    tcp::socket &socket() { return socket_; }

    void start() { do_read(); }

  private:
    void do_read()
    {
        buffer_.resize(1024);
        socket_.async_read_some(
              buffer(buffer_),
              bind_executor(strand_,
                            [self = shared_from_this()]
                                  (const boost::system::error_code &ec, std::size_t length) {
                                self->handle_read(ec, length);
                            }));
    }

    void handle_read(boost::system::error_code ec, std::size_t length)
    {
        if (!ec) {
            buffer_.resize(length);
            auto endpoint = socket().remote_endpoint();
            if (!buffer_.empty())
                std::cout << endpoint.address().to_string() << ":"
                          << endpoint.port() << " : " << buffer_.data() << "\n";

            async_write(socket_, buffer(buffer_),
                        bind_executor(strand_,
                                      [self = shared_from_this()]
                                            (const boost::system::error_code &ec, std::size_t) {
                                          if (!ec) {
                                              self->do_read();  // Continue chain
                                          } else {
                                              std::cerr << "Write error: " << ec.message() << "\n";
                                          }
                                      }));
        } else if (ec == error::eof) {
            auto endpoint = socket().remote_endpoint();
            std::cout << endpoint.address().to_string() << ":"
                      << endpoint.port() << " Disconnected cleanly\n";
        } else {
            auto endpoint = socket().remote_endpoint(ec);
            std::cout << endpoint.address().to_string() << ":"
                      << endpoint.port() << " was disconnected unexpectedly\n"
                      << "Read error: " << ec.message() << "\n";
        }
    }

    tcp::socket socket_;
    std::vector<char> buffer_;
    std::atomic<int> active_connections{0};
    strand<any_io_executor> strand_;
};

// Server class
class server
{
  public:
    server(io_context &io_ctx, short port)
          : acceptor_(io_ctx, tcp::endpoint(tcp::v4(), port))
    {
        start_accept();
    }


  private:
    tcp::acceptor acceptor_;

    void start_accept()
    {
        auto new_session = std::make_shared<session>(acceptor_.get_executor());

        acceptor_.async_accept(
              new_session->socket(),
              [this, new_session](const boost::system::error_code &ec) {
                  handle_accept(new_session, ec);
              });
    }

    void handle_accept(std::shared_ptr<session> new_session,
                       const boost::system::error_code &ec)
    {
        if (!ec) {
            auto endpoint = new_session->socket().remote_endpoint();
            std::cout << "New client connected: "
                      << endpoint.address().to_string() << ":"
                      << endpoint.port() << "\n";
            new_session->start();
//                std::cout << "New client connected\n";
        } else {
            std::cerr << "Accept error: " << ec.message() << "\n";
        }
        start_accept();  // Accept next
    }
};


int main()
{
    try {
        io_context io_ctx;

        server s(io_ctx, 12345);

        std::cout << "Async echo server running on port 12345 (Ctrl+C to stop)\n";

        io_ctx.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}