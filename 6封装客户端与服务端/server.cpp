#include "EasyTcpServer.hpp"
int main()
{
	EasyTcpServer server;
	server.InitSocket();
	server.BindPort(nullptr, 4567);
	server.ListenPort(5);
	
	while (server.isRun())
	{
		server.OnRun();
	}
	server.Close();
	return 0;
}