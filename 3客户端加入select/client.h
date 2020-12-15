#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//避免引入早期库，引起冲突
#include<WinSock2.h>
#include<Windows.h>
#include<stdio.h>
//windows下
//#pragma comment(lib,"ws2_32.lib")
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
struct DataHeaader
{
	short dataLength;
	short cmd;
};
struct Login :public DataHeaader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];

};
struct LoginResult :public DataHeaader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};
struct LogOut :public DataHeaader
{
	LogOut()
	{
		dataLength = sizeof(LogOut);
		cmd = CMD_LOGOUT;

	}
	char userName[32];
};
struct LogoutResult :public DataHeaader
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
struct NewUser :public DataHeaader
{
	NewUser()
	{
		dataLength = sizeof(NewUser);
		cmd = CMD_NEW_USER_JOIN;
		sock_id = 0;
	}
	int sock_id;
};
int ProcessorFunction(SOCKET _csock)
{
	char szRecv[1024] = {};
	//接收客户端请求
	int nlen = recv(_csock, szRecv, sizeof(DataHeaader), 0);
	DataHeaader *_header = (DataHeaader*)szRecv;
	if (nlen <= 0)
	{
		printf("disconnect...\n", _csock);
		return -1;
	}
	switch (_header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(_csock, szRecv + sizeof(DataHeaader), _header->dataLength - sizeof(DataHeaader), 0);
			LoginResult *Loginres = (LoginResult*)szRecv;
			printf("Received server data CMD_LOGIN_RESULT:  Data Length= %d \n", Loginres->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(_csock, szRecv + sizeof(DataHeaader), _header->dataLength - sizeof(DataHeaader), 0);
			LogoutResult *Logoutres = (LogoutResult*)szRecv;
			printf("Received server data CMD_LOGOUT_RESULT:  Data Length= %d \n", Logoutres->dataLength);
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			recv(_csock, szRecv + sizeof(DataHeaader), _header->dataLength - sizeof(DataHeaader), 0);
			NewUser *newUserJoin = (NewUser*)szRecv;
			printf("Received server data CMD_NEW_USER_JOIN:  Data Length= %d \n", newUserJoin->dataLength);

		}
		break;
	}
}
