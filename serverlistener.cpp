#include <ctime>
#include <iostream>
#include <functional>
#include <memory>
#include <string>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
//tcp connection

class tcp_connection
  : public std::enable_shared_from_this<tcp_connection>{
    public:
  typedef std::shared_ptr<tcp_connection> pointer;
     static pointer create(asio::io_context& io_context)
  {
    return pointer(new tcp_connection(io_context));
  }

  asio::ip::tcp::socket& socket()
  {
    return socket_;
  }
   void start() {
        do_read();
    }

    private:
     tcp_connection(asio::io_context& io_context)
        : socket_(io_context) {}
    void do_read() {
       auto self = shared_from_this();
    asio::async_read_until(socket_, asio::dynamic_buffer(message_), '\n',
        [self](std::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                std::string received_data = self->message_.substr(0, bytes_transferred);

                std::time_t now = std::time(nullptr);
                std::cout << "[" << std::ctime(&now) << "] Received from client: " 
                         << received_data << std::endl;

                std::cout << "Bytes transferred: " << bytes_transferred << std::endl;
                std::cout << "Raw data length: " << self->message_.length() << std::endl;

                self->message_.clear();
                self->do_read();
            } else {
                std::cerr << "Read error: " << ec.message() << std::endl;
            }
        });
    }
    asio::ip::tcp::socket socket_;
    std::string message_;
  };

//server Class

class server{
    public:
    server(asio::io_context& io_context): io_context_(io_context), acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 1234)){
        start_accept();
    }
    private:
    void start_accept(){
      tcp_connection::pointer new_connection = tcp_connection::create(io_context_);
      acceptor_.async_accept(new_connection->socket(), std::bind(&server::handle_accept, this, new_connection,std::placeholders::_1));

    }
    void handle_accept(tcp_connection::pointer  new_connection,
      const std::error_code& error)
  {
    if (!error)
    {
      new_connection->start();
    }

    start_accept();
  }

  asio::io_context& io_context_;
  asio::ip::tcp::acceptor acceptor_;
};
int main(){
   try {
        asio::io_context io_context;
        server s(io_context);

        std::cout << "Server is running on port 1234..." << std::endl;
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}