#ifndef MYSERVER_H
#define MYSERVER_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif // !_WIN32_WINNT

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif//ASIO_STANDALONE

#include"Session.h"
#include <asio.hpp>
#include<thread>
#include<map>
class MyServer
{
public:
    MyServer();
    ~MyServer();
    void init();
    void start();
    void onStart();
    void run();
    void wait();
    void stop();
    void command();
    void removeSession(int id);
private:
    void do_accept();
    void accept_handler(Session *session, const asio::error_code &error);
private:
    asio::io_service *m_io_service;
    asio::io_service::work *m_service_work;//
    asio::ip::tcp::endpoint* m_endpoint;
    asio::ip::tcp::acceptor * m_acceptor;
    asio::ip::tcp::socket * m_socket;
    std::thread * m_thread;
    asio::error_code m_ec;

    bool m_running;
    int m_currentSessionId ;
    std::map<int, Session*> m_sessionMap;
};

#endif//M