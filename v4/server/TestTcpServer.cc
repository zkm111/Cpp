 ///
 /// @file    TestAcceptor.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2019-05-07 15:42:14
 ///
 
#include "Threadpool.h"
#include "TcpServer.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <functional>
using namespace std;
using namespace wd;
using namespace std::placeholders;
class EchoServer
{
public:
	EchoServer(const string & ip,unsigned short port,size_t threadNum,size_t queSize)
		:_threadpool(threadNum,queSize)
		,_server(ip,port)
	{}
	void start()
	{
		_server.setConnectionCallback(std::bind(&EchoServer::onConnection,this,_1));
		_server.setMessageCallback(std::bind(&EchoServer::onMessage,this,_1));
		_server.setCloseCallback(std::bind(&EchoServer::onClose,this,_1));
		_threadpool.start();
		_server.start();
	}
	void onConnection(const TcpConnectionPtr & conn);
	void onMessage(const TcpConnectionPtr & conn);
	void onClose(const TcpConnectionPtr & conn);
	void process(const string & msg,const TcpConnectionPtr & conn);//业务逻辑的处理

private:
	Threadpool _threadpool;
	TcpServer _server;
};

void EchoServer::onConnection(const TcpConnectionPtr & conn)
{
	cout << conn->toString() << " has connected!" << endl;
	conn->send("welcome to server.");
}

void EchoServer::onMessage(const TcpConnectionPtr & conn)
{
	cout << "onMessage...." << endl;
	string msg = conn->receive();
	cout << ">> receive msg from client: " << msg << endl;
	_threadpool.addTask(std::bind(&EchoServer::process,this,msg,conn));
}

void EchoServer::onClose(const TcpConnectionPtr & conn)
{
	cout << "onClose...." << endl;
	cout << conn->toString() << " has closed!" << endl;
}

void EchoServer::process(const string & msg,const TcpConnectionPtr & conn)
{
	string response=msg;
	conn->sendInLoop(response);
}

int main(void)
{

	EchoServer server("192.168.3.51",8888,4,10);
	server.start();
	return 0;
}
