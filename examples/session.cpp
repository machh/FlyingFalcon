#include"Session.h"

Session::Session(asio::io_service* arg_ios)
{
    m_socket=new asio::ip::tcp::socket(*arg_ios);
    read_data=new char [128];
}
void Session::start()
{
}
void Session::reset(){
    m_socket->close();
}
void Session::read()
{
    m_socket->async_read_some(asio::buffer(read_data, 128),std::bind(&Session::on_read,this,std::placeholders::_1,std::placeholders::_2));
}
void Session::on_read(const asio::error_code &error, size_t bytes_transferred){
    if (error || bytes_transferred == 0)
    {
        std::cout<<"session-id:"<<getId()<<" is closed!"<<std::endl;
        return;
    }else{
        std::cout<<"session-id:"<<getId()<<" [msg]>>"<<read_data<<std::endl;
    }
    read();
}
void Session::write(char * data)
{
    m_socket->async_write_some(asio::buffer(data, 128),std::bind(&Session::on_write,this,data,std::placeholders::_1,std::placeholders::_2));
}
void Session::on_write(char *data ,const asio::error_code &error, size_t bytes_transferred)
{
    if (error || bytes_transferred == 0)
    {
        std::cout<<"session-id:"<<getId()<<" is closed!"<<std::endl;
        return;
    }else{
        std::cout<<"send-by-session-id:"<<getId()<<" [msg]>>"<<read_data<<std::endl;
    }
}
void Session::setId(int id){
    m_sessionId=id;
}
int Session::getId(){
    return m_sessionId;
}
asio::ip::tcp::socket* Session::getSocket()
{
    return m_socket;
}
Session::~Session()
{
    std::cout<<"session-"<<getId()<<" is release!\n";
}