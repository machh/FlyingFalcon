
#include "TcpClient.h"
 
TcpClient::TcpClient(void)
{
	m_uiSendBufferSize = 0;
	m_uiSendTimeout = 10000;
	m_uiRecvBufferSize = 0;
	m_uiRecvTimeout = 10000;
	m_pEndPoint = NULL;
	m_pSocket = NULL;
	m_nSyncRecviceDataSize = 0;
	m_pTimer = new deadline_timer(m_io);
}
 
TcpClient::~TcpClient(void)
{
}
 
//设置参数
void	TcpClient::SetParameter(unsigned int uiSendBufferSize,unsigned int uiSendTimeout,unsigned int uiRecvBufferSize,unsigned int uiRecvTimeout)
{
	m_uiSendBufferSize = uiSendBufferSize;
	m_uiSendTimeout = uiSendTimeout;
	m_uiRecvBufferSize = uiRecvBufferSize;
	m_uiRecvTimeout = uiRecvTimeout;
	if(m_uiRecvTimeout <= 0)
	{
		m_uiRecvTimeout = 10000;
	}
}
 
 
//连接服务器(同步)
int	TcpClient::ConnectServer(char *pIp,unsigned short usPort,unsigned int uiConnectTimeout)
{
	if(pIp == NULL || usPort == 0)
		return -1;
 
	try
	{
		m_pEndPoint = new ip::tcp::endpoint(ip::address::from_string(pIp), usPort);
		m_pSocket = new ip::tcp::socket(m_io);
		m_pSocket->open(m_pEndPoint->protocol());
		//m_pSocket->set_option(boost::asio::ip::tcp::socket::reuse_address(true));
		if(m_uiSendBufferSize != 0)
		{
			boost::asio::socket_base::send_buffer_size sendBufferSize(m_uiSendBufferSize);
			m_pSocket->set_option(sendBufferSize);
		}
		if(m_uiRecvBufferSize != 0)
		{
			boost::asio::socket_base::receive_buffer_size recvBufferSize(m_uiRecvBufferSize);
			m_pSocket->set_option(recvBufferSize);
		}
		//connect
		m_pSocket->connect(*m_pEndPoint);
		/*
		char str[1024];
		sock.read_some(buffer(str));
		std::cout << "receive from" << sock.remote_endpoint().address() << std::endl;;
		std::cout << str << std::endl;
		*/
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return -2;
	}
 
	return 0;
}
 
//连接服务器(异步)
int	TcpClient::ConnectServerByAynsc(char *pIp,unsigned short usPort,unsigned int uiConnectTimeout,unsigned int uiReconnectInteralTime)
{
	if(pIp == NULL || usPort == 0)
		return -1;
 
	try
	{
		m_pEndPoint = new ip::tcp::endpoint(ip::address::from_string(pIp), usPort);
		m_pSocket = new ip::tcp::socket(m_io);
		m_pSocket->open(m_pEndPoint->protocol());
		//m_pSocket->set_option(boost::asio::ip::tcp::socket::reuse_address(true));
		if(m_uiSendBufferSize != 0)
		{
			boost::asio::socket_base::send_buffer_size sendBufferSize(m_uiSendBufferSize);
			m_pSocket->set_option(sendBufferSize);
		}
		if(m_uiRecvBufferSize != 0)
		{
			boost::asio::socket_base::receive_buffer_size recvBufferSize(m_uiRecvBufferSize);
			m_pSocket->set_option(recvBufferSize);
		}
		//connect
		m_pSocket->async_connect(*m_pEndPoint,boost::bind(&TcpClient::connect_handler,this, boost::asio::placeholders::error));
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return -2;
	}
 
	return 0;
}
 
void TcpClient::connect_handler(const boost::system::error_code& ec)
{
	if (ec)
	{
		return;
	}
 
	std::cout << "receive from:" << m_pSocket->remote_endpoint().address() << std::endl;
}
 
void TcpClient::async_read_handler(const boost::system::error_code& ec,size_t bytes_transferred,TcpRecvDataCallback fnCallback,DWORD dwUserData1,DWORD dwUserData2)
{
	//回调数据
	if(fnCallback != NULL)
	{
		fnCallback(ec,m_rbTempRecvBuffer.data(),bytes_transferred,dwUserData1,dwUserData2);
	}
 
	if(ec == boost::asio::error::eof)
	{
		//对端方关闭连接
		if(m_pSocket->is_open())
		{
			m_pSocket->close();
		}
		//printf("close connect \n");
		return;
	}
	if(ec != NULL)
	{
		//发送数据失败
		return;
	}
 
	//接收下一条数据
	m_pSocket->async_read_some(boost::asio::buffer(m_rbTempRecvBuffer),
		boost::bind(&TcpClient::async_read_handler, this, 
		boost::asio::placeholders::error, 
		boost::asio::placeholders::bytes_transferred,
		fnCallback,dwUserData1,dwUserData2));
}
 
//关闭连接
void	TcpClient::CloseConnect()
{
	if(m_pSocket != NULL)
	{
		m_pSocket->close();
		m_pSocket = NULL;
	}
}
 
 
//发送数据
int		TcpClient::SendData(char *pBuffer,int nBufferSize)
{
	int nRet = 0;
	if(m_pSocket != NULL)
	{
		nRet = m_pSocket->send(buffer(pBuffer,nBufferSize));
	}
 
	return nRet;
}
 
//接收数据
int		TcpClient::RecvData(char *pBuffer,int nBufferSize)
{
	int nRet = 0;
	if(m_pSocket != NULL)
	{
		m_nSyncRecviceDataSize = 0;
		boost::system::error_code ec;
		m_pSocket->async_read_some(buffer(pBuffer,nBufferSize),boost::bind(&TcpClient::read_hander,this,pBuffer,boost::asio::placeholders::bytes_transferred,boost::asio::placeholders::error));
		m_pTimer->expires_from_now(boost::posix_time::seconds(m_uiRecvTimeout));  
		m_pTimer->async_wait(boost::bind(&TcpClient::RecvDataTimeoutProcess,this));
		m_io.reset();
		m_io.run(ec);
		nRet = m_nSyncRecviceDataSize;
	}
 
	return nRet;
}
 
void TcpClient::read_hander(char *pBuffer,size_t bytes_transferred,const boost::system::error_code& err)
{
	if (err)
	{
		return;
	}
	m_nSyncRecviceDataSize = bytes_transferred;
	m_pTimer->cancel();
} 
 
void TcpClient::RecvDataTimeoutProcess()
{
	int n = 0;
}
 
//接收数据(阻塞)
int	TcpClient::RecvDataByBlock(char *pBuffer,int nBufferSize)
{
	int nRet = 0;
	if(m_pSocket != NULL)
	{
		m_nSyncRecviceDataSize = 0;
		boost::system::error_code ec;
		m_pSocket->receive(buffer(pBuffer,nBufferSize));
	}
 
	return nRet;
}
 
//发送数据(异步)
int		TcpClient::SendDataByAynsc(char *pBuffer,int nBufferSize,TcpSendDataCallback fnCallback,DWORD dwUserData1,DWORD dwUserData2)
{
	if(pBuffer == NULL || nBufferSize == 0)
		return -1;
 
	if(m_pSocket == NULL || !m_pSocket->is_open())
		return -1;
 
	boost::asio::async_write(
		*m_pSocket, 
		boost::asio::buffer(pBuffer,nBufferSize), 
		boost::bind(&TcpClient::write_handler, this,
		boost::asio::placeholders::error, 
		boost::asio::placeholders::bytes_transferred,
		fnCallback,dwUserData1,dwUserData2));
 
	return 0;
}
 
//接收数据(异步)
int		TcpClient::RecvDataByAynsc(TcpRecvDataCallback fnCallback,DWORD dwUserData1,DWORD dwUserData2)
{
	if(m_pSocket == NULL || !m_pSocket->is_open())
		return -1;
 
	m_pSocket->async_read_some(buffer(m_rbTempRecvBuffer),boost::bind(&TcpClient::async_read_handler, this, boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,fnCallback,dwUserData1,dwUserData2));
	
	return 0;
}
 
//设置阻塞与非阻塞
void TcpClient::SetNoBlock(bool bNoBlock)
{
	if(m_pSocket == NULL)
		return;
 
	if(bNoBlock)
	{
		boost::asio::ip::tcp::socket::non_blocking_io io_option(true);
		m_pSocket->io_control(io_option);
	}
	else
	{
		//阻塞
		boost::asio::ip::tcp::socket::non_blocking_io io_option(false);
		m_pSocket->io_control(io_option);
	}
}
 
void TcpClient::write_handler(const boost::system::error_code& error,size_t bytes_transferred,TcpSendDataCallback fnCallback,DWORD dwUserData1,DWORD dwUserData2)
{
	if(fnCallback != NULL)
	{
		fnCallback(error,bytes_transferred,dwUserData1,dwUserData2);
	}
	if(error == boost::asio::error::eof)
	{
		//对端方关闭连接
		if(m_pSocket->is_open())
		{
			m_pSocket->close();
		}
		//printf("close connect \n");
		return;
	}
	if(error != NULL)
	{
		//发送数据失败
		return;
	}
 
#ifdef _DEBUG
	//写数据
	printf("write data!!!\n");
#endif
}