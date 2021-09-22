 
#define ASIO_STANDALONE

#include <iostream>
#include <asio.hpp>
#include<thread>
using asio::ip::tcp;

void read(tcp::socket *s) {
    char buf[128];
     asio::error_code error;
     while(true) {
        s->read_some(asio::buffer(buf), error);
        if (error) {
            std::cout<<"session closed!\n";
            break;
        }
        std::cout<<"[msg]>>"<<buf<<std::endl;
     }
}

int main(int argc, char* argv[])
{
    const char* server_ip="127.0.0.1";
    const char* server_port="54545";
  
  try {
       asio::io_service io_service;

       tcp::resolver resolver(io_service);

       tcp::resolver::query query(server_ip,server_port);
       tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

       tcp::socket socket(io_service);

       asio::connect(socket, endpoint_iterator);
       std::thread readThread(std::bind(read,&socket));
       char buf[128];
        while (std::cin.getline(buf,128)) {
              size_t len = std::strlen(buf);
              buf[len]='\0';
              asio::error_code error;
              socket.write_some(asio::buffer(buf), error);
              if (error == asio::error::eof)
                   break; // Connection closed cleanly by peer.
              else if (error)
                  throw asio::system_error(error); // Some other error.
         }
         readThread.join();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    system("pause");
    return 0;
}