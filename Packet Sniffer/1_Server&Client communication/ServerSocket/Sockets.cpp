#define _WIN32_WINNT 0x0600
#include<iostream>
#include<Winsock2.h>
#include <WS2tcpip.h> 
#include<string>
int main()
{
	std::cout << "Winsock SERVER Application" << std::endl;
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
	SOCKET srvSocket = INVALID_SOCKET;
	srvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (srvSocket == INVALID_SOCKET)
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
	int port = 5555;
	service.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &service.sin_addr.s_addr);
	service.sin_port = htons(port);
	if (bind(srvSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		std::cout << "Bind id failed." << WSAGetLastError() << std::endl;
		closesocket(srvSocket);
		WSACleanup();
		return -1;
	}
	else
	{
		std::cout << "Bind is ok !" << std::endl;
	}
	if (listen(srvSocket, 1) == SOCKET_ERROR)
	{
		std::cout << "Listenning Failed with error :" << WSAGetLastError() << std::endl;
	}
	else
	{
		std::cout << "Listenning..." << std::endl;
	}
	SOCKET AcceptSocket;
	AcceptSocket = accept(srvSocket, NULL, NULL);
	if (AcceptSocket == INVALID_SOCKET)
	{
		std::cout << "Accept Failed with error :" << WSAGetLastError() << std::endl;
		WSACleanup();
		return -1;
	}
	std::cout << "Accepted !" << std::endl;

	char buffer[200];
	char confirmation[200] = "message received";
	memset(buffer, 0, sizeof(buffer));
	do {
		int bytecount = recv(AcceptSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytecount == SOCKET_ERROR)
		{
			std::cerr << "receive Failed :" << WSAGetLastError() << std::endl;
			return -1;
		}
		else if (bytecount == 0)
		{
			std::cout << "Socket disconected." << std::endl;
			return 0;
		}
		else {
			std::string receivedData(buffer, bytecount);
			std::cout << "Received (" << bytecount << " bytes): " << receivedData << std::endl;
		}
		bytecount = send(AcceptSocket, confirmation, strlen(confirmation), 0);
		if (bytecount == SOCKET_ERROR)
		{
			WSACleanup();
			return -1;
		}
		else
			std::cout << "Send verification meesage ..." << std::endl;
		std::string message;
		std::cout << "Enter your message (type 'quit' to exit):";
		std::getline(std::cin, message);
		if (message == "quit" || message == "exit")
		{
			std::cout << "Shutting down..." << std::endl;
			break;
		}
		const char* msgBuffer = message.c_str();
		int length = static_cast<int>(message.length());
		int bytecount2 = send(AcceptSocket, msgBuffer, length, 0);
		if (bytecount2 == INVALID_SOCKET)
		{
			std::cout << "Sending failed with error :" << WSAGetLastError() << std::endl;

		}
		else
			std::cout << "Send (" << bytecount2 << " bytes)" << std::endl;
	} while (true);

	closesocket(AcceptSocket);
	WSACleanup();
	return 0;
}