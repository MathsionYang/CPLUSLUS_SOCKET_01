#ifndef _EasyTcpServer_hpp_
#define _EasyTcpCLient_hpp_
#include "server.h"
class EasyTcpServer
{
private:
	SOCKET _sock;
	vector<SOCKET>g_client;
public:
	EasyTcpServer()
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~EasyTcpServer()
	{
		Close();
	}
	//初始化SOCKET
	void InitSocket() {
		//启动socket2
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
		//建立Socket套接字
		if (INVALID_SOCKET != _sock)
		{
			printf("close old socket\n");
			Close();
		}
		 _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		 if (INVALID_SOCKET == _sock)
		 {
			 printf("build socket failed\n");
		 }
		 else
		 {
			 printf("build socket successed\n");
		 }
	}
	//绑定ip端口号
	void BindPort(const char* ip,short port)
	{
	
		if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
		if (ip)
		{
			_sin.sin_addr.S_un.S_addr = inet_addr(ip);////不限定访问Ip地址
		}
		else
		{
			_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");////不限定访问Ip地址
		}
		int ret = bind(_sock, (sockaddr*)&_sin, sizeof(_sin));
		if (ret == SOCKET_ERROR)
		{
			printf("Bind Port failed,Port<%d>\n", port);
		}
		else
		{
			printf("Bind Port Sucessed\n");
		}
	}
	//监听端口号
	void ListenPort(int n)
	{
		//listen监听网络端口
		int ret = listen(_sock, n);
		if (ret == SOCKET_ERROR)
		{
			printf("<socket=%d>Listen Port failed\n",_sock);
		}
		else
		{
			printf("<socket=%d>Listen Port Sucessed\n", _sock);
		}
	}

	//接收客户端信息
	void AcceptClient()
	{
		sockaddr_in clientAddr = {};
		int nAddrLen = sizeof(sockaddr_in);
		SOCKET _csock = INVALID_SOCKET;

		_csock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
		if (_csock == INVALID_SOCKET) {
			printf("failed，Invalid client received\n");
		}
		else
		{
			NewUser userJoin;
			Send2ALLData(&userJoin);
			printf("New Client:SOCKET = %d  IP = %s\n", _csock, inet_ntoa(clientAddr.sin_addr));
			g_client.push_back(_csock);
			std::cout << "_csock_value" << _csock << std::endl;
		}
		
	}
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdRead;
			FD_ZERO(&fdRead);
			FD_SET(_sock, &fdRead);
			for (int i = g_client.size() - 1; i >= 0; i--)
			{
				FD_SET(g_client[i], &fdRead);
			}
			timeval time = { 1,0 };
			int ret = select(_sock + 1, &fdRead,0,0, &time);
			if (ret < 0)
			{
				printf("failed，closed\n");
				Close();
				return false;
			}
			if (FD_ISSET(_sock, &fdRead))
			{
				FD_CLR(_sock, &fdRead);
				//accept等待客户端链接
				AcceptClient();
				return true;
			}
			for (int k = 0; k < g_client.size(); k++)
			{
				if (FD_ISSET(g_client[k], &fdRead))
				{
					if (-1 == RecvData(g_client[k]))
					{
						auto iter = find(g_client.begin(), g_client.end(), fdRead.fd_array[k]);
						if (iter != g_client.end())
						{
							g_client.erase(iter);
						}
					}
				}
				
			}
			//printf("other work doing\n");

			return true;
		}
		return false;
		
	}
	//关闭socket
	void Close()
	{
		//关闭Win10 Socket环境
		for (int n = (int)g_client.size() - 1; n >= 0; n--)
		{
			closesocket(g_client[n]);
		}
		// 8 关闭套节字closesocket
		closesocket(_sock);
		//------------
		//清除Windows socket环境
		WSACleanup();
	}
	int RecvData(SOCKET _csock)
	{
		char szRecv[4096] = {};
		//接收客户端请求
		int nlen = recv(_csock, szRecv, sizeof(DataHeaader), 0);
		DataHeaader *_header = (DataHeaader*)szRecv;
		if (nlen <= 0)
		{
			printf("client<Socket=%d> end\n", _csock);
			return -1;
		}
		recv(_csock, szRecv + sizeof(DataHeaader), _header->dataLength - sizeof(DataHeaader), 0);
		OnNetMsg(_csock, _header);
		return 0;

	}
	virtual void OnNetMsg(SOCKET _csock,DataHeaader *_header)
	{

		switch (_header->cmd)
		{
		case CMD_LOGIN:
		{
			Login *login = (Login*)_header;
			printf("Received CLient<Socket=%d>command:  Data Length= %d  user = %s pass = %s\n", _csock, login->dataLength, login->userName, login->passWord);
			LoginResult res;
			send(_csock, (char*)&res, sizeof(LoginResult), 0);

		}
		break;
		case CMD_LOGOUT:
		{

			LogOut *logout = (LogOut*)_header;
			printf("Received command: user=%s\n", logout->userName);
			LogoutResult _lgres;
			send(_csock, (char*)&_lgres, sizeof(LogoutResult), 0);

		}
		break;
		default:
			DataHeaader hd = { 0,CMD_ERROR };
			send(_csock, (const char*)&hd, sizeof(hd), 0);
			break;
		}
	
	}
	//发送指定SOCKET数据
	int SendData(SOCKET _csock, DataHeaader *_header)
	{
		if (isRun() && _header)
		{
			return send(_csock, (const char*)_header, _header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}
	void Send2ALLData( DataHeaader *_header)
	{
			for (int n = g_client.size() - 1; n >= 0; n--)
			{
				SendData(g_client[n], _header);
			}
	}

private:

};



#endif