
#ifndef SESSION_H
#define SESSION_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif // !_WIN32_WINNT

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif//ASIO_STANDALONE


#include<asio.hpp>
#include<iostream>
class Session
{
public:
    explicit Session(asio::io_service* arg_ios);
    ~Session();
    void start();
    void reset();//重置会话
    void read();//异步接收消息
    void on_read(const asio::error_code &error, size_t bytes_transferred);
    void write(char * data);
    void on_write(char *data ,const asio::error_code &error, size_t bytes_transferred);
    asio::ip::tcp::socket* Session::getSocket();
    void setId(int id);
    int getId();
private:
        asio::ip::tcp::socket *m_socket;
        int m_sessionId;
        char * read_data;
};
#endif//SESSION_H