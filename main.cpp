
#include <ctime>
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <mutex>
#include <asio.hpp>

using asio::ip::tcp;
std::atomic_bool g_running {true};

//asio::ip::tcp::endpoint server_addr;
//asio::ip::tcp::acceptor acceptor;

std::string make_daytime_string() {
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

int main() {
    // SIGINT handler
    signal(SIGINT, [](int sig) {
        g_running = false;
        std::cout << "ctrl+c is pressed .. exit...." <<std::endl;
    });
    
    try {
        asio::io_context io_context;
       
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8899));
        
        std::cout << "loccal addr info: \n addr: " << acceptor.local_endpoint().address() << std::endl;
        std::cout << "port: " << acceptor.local_endpoint().port() << std::endl;
        
        while (g_running) {
             tcp::socket socket(io_context);
             acceptor.accept(socket);
             std::string message = make_daytime_string();
             std::cout << "msg : " << message  <<std::endl;
            
             std::cout << "peer client addr = " << socket.remote_endpoint().address() << std::endl;
            

             asio::error_code ignored_error;
             asio::write(socket, asio::buffer(message), ignored_error);
             //socket.read_some(asio::buffer(message));   //sync
        }
     } catch (std::exception& e) {
         std::cerr << e.what() << std::endl;
     }

     std::cout << "main process exit.... " <<std::endl;
     return 0;
}
