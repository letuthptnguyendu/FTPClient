#include "ClientSocket.h"


ClientSocket::ClientSocket(int clientPort, std::string clientIP = "0.0.0.0")
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(clientIP.c_str());
	addr.sin_port = htons(clientPort);

	int len = sizeof(sockaddr);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bind(sock, (sockaddr*)&addr, len);
}


ClientSocket::~ClientSocket()
{
	closesocket(sock);
}

ClientSocket::ClientSocket() 
{
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		throw "WSAStartup failed with error";
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		throw "socket failed with error";
		WSACleanup();
	}
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

int ClientSocket::getSocketPort() {
	return socketPort;
}

void ClientSocket::connectTo(std::string serverIP, int serverPort) {
	//server
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(serverIP.c_str());
	server.sin_port = htons(serverPort);

	//connect
	iResult = connect(sock, (SOCKADDR *)& server, sizeof(server));
	if (iResult == SOCKET_ERROR) {
		throw "connect function failed with error";
		WSACleanup();
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

void ClientSocket::sendRequest(ClientRequest request) {
	std::string full_cmd_request = request.getFullCmd();
	if (send(sock, full_cmd_request.c_str(), full_cmd_request.length(), 0) == 0) {
		throw "Can't send";
	}
}

void ClientSocket::sendData(std::string data) {
	if (send(sock, data.c_str(), data.length(), 0) == 0) {
	
	}
}

void ClientSocket::acceptSock(std::string ip, int port) {
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip.c_str());
	server.sin_port = htons(port);

	int len = sizeof(server);

	sock = accept(sock, (sockaddr*)&server, &len);
}

void ClientSocket::listenSock() {
	if (listen(sock, 1) == SOCKET_ERROR) {
		throw "nat";
	}
}