#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

class telemetry {
public:
	int value;
	char date[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	bool finalValue = FALSE;
};

int main() {
	//step 1: initialization of Winsock
	WSAData wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	int wsaErr = WSAStartup(wVersionRequested, &wsaData);
	if (wsaErr != 0) {
		std::cout << "Winsock DLL not found" << std::endl;
		return 0;
	}
	else {
		std::cout << "Winsock DLL found\n";
		std::cout << "Status: " << wsaData.szSystemStatus << "\n" << std::endl;
	}

	//step 2: creating socket
	SOCKET serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		std::cout << "Error at socket() " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "socket() is ready\n" << std::endl;
	}

	//step 3: binding
	sockaddr_in service;
	int PORT = 55555;
	service.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
	service.sin_port = htons(PORT);
	if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
	}
	else {
		std::cout << "bind() is ready\n" << std::endl;
	}

	//step 4: listening
	if (listen(serverSocket, 1) == SOCKET_ERROR) {
		std::cout << "listen(): Error listening on socket " << WSAGetLastError() << std::endl;
	}
	else {
		std::cout << "listen() is ready, waiting for connections...\n" << std::endl;
	}

	//step 5: accepting
	SOCKET acceptSocket = accept(serverSocket, NULL, NULL); //check
	if (acceptSocket == INVALID_SOCKET) {
		std::cout << "accept() failed: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return -1;
	}
	std::cout << "Accepted connection" << std::endl;

	//chat to the client
	telemetry temp;
	do {
		int byteCount = recv(acceptSocket, (char*)&temp, sizeof(temp), 0);
		if (byteCount > 0) {
			std::cout << "Message received!\nTemperature: " << temp.value << "\nDate: " << temp.date << std::endl;
			send(acceptSocket, "Message received", sizeof("Message received"), 0);
		}
		else {
			send(acceptSocket, "Message delivery failed", sizeof("Message delivery failed"), 0);
			WSACleanup();
		}
	} while (temp.finalValue != TRUE);
	

	system("pause");
	WSACleanup();
	return 0;
}