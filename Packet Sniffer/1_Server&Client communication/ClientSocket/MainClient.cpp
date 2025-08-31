#define _WIN32_WINNT 0x0600
#include<iostream>
#include<Winsock2.h>
#include <WS2tcpip.h> 
#include<string>

int main()
{
	std::cout << "Winsock Client Application" << std::endl;
	WSADATA wsaDATA;
	int wsaerr;
	WORD wVersionRequested = MAKEWORD(2, 2);
	wsaerr = WSAStartup(wVersionRequested, &wsaDATA);
	if (wsaerr != 0)
	{
		std::cout << "WinSock dll not found." << std::endl;
	}
	else
	{
		std::cout << "WinSock dll found !" << std::endl;
		std::cout << "status :" << wsaDATA.szSystemStatus << std::endl;
	}
	SOCKET ClientSocket = INVALID_SOCKET;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cout << "Socket() creation failed :" << WSAGetLastError() << std::endl;
		WSACleanup();
		return -1;
	}
	else
	{
		std::cout << "Socket() created." << std::endl;
	}
	sockaddr_in service;
	service.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &service.sin_addr.s_addr);
	service.sin_port = htons(5555);
	if (connect(ClientSocket, (SOCKADDR*)&service, sizeof(service)) == INVALID_SOCKET)
	{
		std::cout << "Connection Failed." << std::endl;
		WSACleanup();
		return -1;
	}
	else
	{
		std::cout << "Connection Start ! " << std::endl;
	}
	do {
		std::string message;
		std::cout << "Enter your message (type 'quit' to exit):";
		std::getline(std::cin, message);
		if (message == "quit" || message == "exit")
		{
			break;
		}
		if (!message.empty())
		{
			const char* buffer = message.c_str();
			int length = static_cast<int>(message.length());
			int bytecount = send(ClientSocket, buffer, length, 0);
			if (bytecount == SOCKET_ERROR)
			{
				std::cout << "Sending message failed with error :" << WSAGetLastError() << std::endl;
				break;
			}
			else {
				std::cout << "Send (" << bytecount << " bytes)" << std::endl;

			}
			char recve[200];
			memset(recve, 0, sizeof(recve));
			int bytecount2 = recv(ClientSocket, recve, sizeof(recve), 0);
			if (bytecount2 == SOCKET_ERROR)
			{
				break;
			}
			else if (bytecount2 == 0)
			{
				std::cout << "Server disconnected." << std::endl;
				break;
			}
			else {
				std::cout << "message from Server : " << recve << std::endl;
			}
			char recve2[200];
			memset(recve2, 0, sizeof(recve2));
			int bytecount3 = recv(ClientSocket, recve2, sizeof(recve2), 0);
			if (bytecount3 == SOCKET_ERROR)
			{
				break;
			}
			else if (bytecount3 == 0)
			{
				std::cout << "Server disconnected." << std::endl;
				break;
			}
			else
			{

				std::cout << "message from server ( " << bytecount3 << " bytes ) :" << recve2 << std::endl;
			}
		}



	} while (true);


	system("pause");
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}