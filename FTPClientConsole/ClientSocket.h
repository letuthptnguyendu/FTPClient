#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <winsock.h>
#include "ClientRequest.h"
#include "ServerResponse.h"
#include "import.h"

#define MAXRECV_BUFFER 2048

class ClientSocket
{
private:
	std::string socketIP;
	int	socketPort;

	SOCKET sock;
	WSADATA wsaData;
	int iResult;

	std::string data;

public:
	//khoi tao 1 socket ket noi toi server
	ClientSocket(int serverPort, std::string serverIP);
	~ClientSocket();

	//khoi tao 1 socket bind 1 dia chi IP va port cu the
	ClientSocket();

	std::string getReponse();
	void sendRequest(ClientRequest request);

	//dung cho socket data -> send data
	void sendData(std::string data);

	//dung cho che do active
	void acceptSock(std::string ip, int port);
	void listenSock();
	//lay port cua control socket -> tao ra port cho datasocket (che do active)
	int getSocketPort();
	void connectTo(std::string svip, int svport);
};

