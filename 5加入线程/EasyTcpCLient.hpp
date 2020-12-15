#ifndef _EasyTcpCLient_hpp_
#define _EasyTcpCLient_hpp_

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>
#include<Windows.h>
#include<stdio.h>
#include <thread>
#include "client.h"
using namespace std;

class EasyTcpCLient
{
	SOCKET _sock;
public:
	EasyTcpCLient()
	{
		_sock = INVALID_SOCKET;
	}
	//����������
	virtual ~EasyTcpCLient()
	{
		Close();
	}
	//��ʼ��socket
	void initSocket()
	{
		//����Win10 Socket����
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
		//����Socket�׽���
		 if (_sock != INVALID_SOCKET)
		 {
			 printf("Close Old <SOCKET=%d> \n",_sock);
			 Close();
		 }
		 _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)
		{
			printf("created SOCKET failed\n");
		}
		else
		{
			printf(" created SOCKET successed\n");
		}
	}
	//���ӷ�����
	int Connect(char*ip,short port)
	{
		if (_sock == INVALID_SOCKET)
		{
			initSocket();
		}
		//���ӷ�����
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);////���޶�����Ip��ַ
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (ret == SOCKET_ERROR)
		{
			printf(" failed connect \n");
		}
		else
		{
			printf("connect successed\n");
		}
		return ret;
	}
	//�ر�socket
	void Close()
	{
		//�ر�Win10 Socket����
		if (_sock != INVALID_SOCKET)
		{
			closesocket(_sock);
			WSACleanup();
			_sock = INVALID_SOCKET;
		}
	}
	//��������
	// ��������
	//����������Ϣ
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdRead;
			FD_ZERO(&fdRead);
			FD_SET(_sock, &fdRead);
			timeval time = { 1,0 };
			int ret = select(_sock, &fdRead, NULL, NULL, &time);
			if (ret < 0)
			{
				printf("failed��<SOCKET=%d>closed1\n", _sock);
				return false;
			}
			if (FD_ISSET(_sock, &fdRead))
			{
				FD_CLR(_sock, &fdRead);
				if (-1 == RecvData(_sock))
				{
					printf("failed��closed2\n");
					return false;
				}
			}
			return true;
		}
		return false;
	}
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}
	//��������
	//����ճ������ְ�
	int RecvData(SOCKET _csock)
	{
		char szRecv[1024] = {};
		//���տͻ�������
		int nlen = recv(_csock, szRecv, sizeof(DataHeaader), 0);
		DataHeaader *_header = (DataHeaader*)szRecv;
		if (nlen <= 0)
		{
			printf("disconnect...\n");
			return -1;
		}
		recv(_csock, szRecv + sizeof(DataHeaader), _header->dataLength - sizeof(DataHeaader), 0);
		OnNetMsg(_header);
		return 0;
	}
	//��Ӧ
	int OnNetMsg( DataHeaader *_header)
	{
		
		switch (_header->cmd)
		{
			case CMD_LOGIN_RESULT:
			{
				
				LoginResult *Loginres = (LoginResult*)_header;
				printf("Received server data CMD_LOGIN_RESULT:  Data Length= %d \n", Loginres->dataLength);
			}
			break;
			case CMD_LOGOUT_RESULT:
			{
				LogoutResult *Logoutres = (LogoutResult*)_header;
				printf("Received server data CMD_LOGOUT_RESULT:  Data Length= %d \n", Logoutres->dataLength);
			}
			break;
			case CMD_NEW_USER_JOIN:
			{
				NewUser *newUserJoin = (NewUser*)_header;
				printf("Received server data CMD_NEW_USER_JOIN:  Data Length= %d \n", newUserJoin->dataLength);

			}
			break;
		}
		return 0;
	}
	//��������
	int SendData(DataHeaader *_header)
	{
		if (isRun() && _header)
		{
			return send(_sock, (const char*)_header, _header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}

private:

};

//EasyTcpCLient::EasyTcpCLient()
//{
//}
//
//EasyTcpCLient::~EasyTcpCLient()
//{
//}

#endif
