#include "MyServer.h"
#include <iostream>
using namespace std;
MyServer::MyServer():m_currentSessionId(0),m_running(false)
{
    m_io_service= new asio::io_service();
    m_service_work=new asio::io_service::work(*m_io_service);
    m_endpoint=new asio::ip::tcp::endpoint(asio::ip::tcp::v4(),54545);
    m_acceptor=new asio::ip::tcp::acceptor(*m_io_service,*m_endpoint);
    m_socket=new asio::ip::tcp::socket(*m_io_service);
}
void MyServer::init(){
}
void MyServer::start(){
    m_running=true;
    m_thread = new std::thread(&MyServer::command, this);
    run();
}
void MyServer::command()
{
    int sid=0;
    char cmd[128];
    while(true){
        std::cin>>sid>>cmd;
        size_t len=std::strlen(cmd);
        cmd[len]='\0';
        if(sid==0){
            std::cout<<"stop cmd!\n";
            break;
        }else{
            Session *s=m_sessionMap.at(sid);
            if(s)   s->write(cmd);
            else    std::cout<<"fail to find session:id="<<sid<<std::endl;
        }
    }
}
void MyServer::onStart()
{
    m_acceptor->listen();
    cout<<"start listening!\n";
    do_accept();
}
void MyServer::run(){
    onStart();
    m_io_service->run();
}
void MyServer::stop(){
    m_running = false;
}
void MyServer::wait(){
    m_thread->join();
}
void MyServer::do_accept(){
    Session *session = new Session(m_io_service);

    if (session)
    {
        //async_accept(socket,fun)--每次建立连接时，新建socket
        m_acceptor->async_accept(*(session->getSocket()),std::bind(&MyServer::accept_handler, this, session, std::placeholders::_1));
    }
    else
    {
        cout<<"allocate client session failed\n";
    }

}
void MyServer::accept_handler(Session *session, const asio::error_code &error)
{
    if (session != NULL && !error)
    {
        m_currentSessionId++;
        session->setId(m_currentSessionId);
        printf("Client Session %d Connected\n", m_currentSessionId);
        m_sessionMap[m_currentSessionId] = session;
        session->read();
    }
    do_accept();
}
void MyServer::removeSession(int id){
    Session *s=m_sessionMap.at(id);
    delete s;
    m_sessionMap.erase(id);
}
MyServer::~MyServer(){
    map<int,Session*>::iterator it;
    for(it=m_sessionMap.begin();it!=m_sessionMap.end();++it){
        delete it->second;
    }
    delete m_service_work;
    delete m_io_service;

    delete m_endpoint;
    delete m_acceptor;
    delete m_socket;
}