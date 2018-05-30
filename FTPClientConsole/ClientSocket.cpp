#include "ClientSocket.h"

ClientSocket::ClientSocket(std::string clientIP, int clientPort)
{
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		std::string e = "WSAStartup failed with error\n";
		throw e;
	}

	//client IP
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(clientIP.c_str());
	addr.sin_port = htons(clientPort);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::string e = "socket failed with error\n";
		throw e;
	}
	bind(sock, (sockaddr*)&addr, sizeof(sockaddr));
}

ClientSocket::ClientSocket()
{
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		std::string e = "WSAStartup failed with error\n";
		throw e;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::string e = "socket failed with error\n";
		throw e;
	}
}

ClientSocket::~ClientSocket()
{
	closesocket(sock);
}


std::string ClientSocket::getReponse() {
	char recvBuffer[MAXRECV_BUFFER];

	int isOk = recv(sock, recvBuffer, MAXRECV_BUFFER, 0);
	if (isOk == 0) {
		return "";
	}
	else {
		data.assign(recvBuffer, isOk);
	}

	return this->data;
}

void ClientSocket::sendRequest(ClientRequest request) {
	std::string full_cmd_request = request.getFullCmd();
	if (send(sock, full_cmd_request.c_str(), full_cmd_request.length(), 0) == SOCKET_ERROR) {
		std::string e = "Send Error\n";
		throw e;
	}
}


void ClientSocket::connectTo(std::string serverIP, int serverPort) {
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(serverIP.c_str());
	server.sin_port = htons(serverPort);

	iResult = connect(sock, (SOCKADDR *)& server, sizeof(server));
	if (iResult == SOCKET_ERROR) {
		std::string e = "connect function failed with error\n";
		throw e;
	}

	//fetch sock ip and sock port
	sockaddr_in addr = { 0 };
	int len = sizeof(addr);
	
	getsockname(sock, (SOCKADDR*)&addr, &len);
	char *ip = inet_ntoa(addr.sin_addr);
	socketIP.assign(ip);
	unsigned int myPort = ntohs(addr.sin_port);
	socketPort = myPort;
}

void ClientSocket::sendData(std::string data) {
	if (send(sock, data.c_str(), data.length(), 0) == SOCKET_ERROR) {
		std::string e = "Send Error\n";
		throw e;
	}
}


void ClientSocket::acceptSock(std::string ip, int port) {
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip.c_str());
	server.sin_port = htons(port);

	int len = sizeof(server);
	//bcs just accept only 1 connector :'>
	sock = accept(sock, (sockaddr*)&server, &len);
	if (sock == INVALID_SOCKET) {
		std::string e = "Accept Fail\n";
		throw e;
	}
}

void ClientSocket::listenSock() {
	if (listen(sock, 1) == SOCKET_ERROR) {
		std::string e = "Listen Fail\n";
		throw e;
	}
}


std::string ClientSocket::getSocketIP() {
	return socketIP;
}

int ClientSocket::getSocketPort() {
	return socketPort;
}