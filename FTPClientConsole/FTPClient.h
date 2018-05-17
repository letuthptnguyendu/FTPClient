#pragma once
#include "ClientSocket.h"
#include "ServerResponse.h"
#include "ClientRequest.h"
#include "UserCmd.h"
#include <Windows.h>
#include <fstream>
#include "utilities.h"


class FTPClient
{
private:
	ClientSocket * controlSocket;
	ClientSocket* dataSocket;

	std::string username;
	std::string password;

	//passive = 1, active = 0
	int mode;
	int subport;

	std::string serverIP;
	int	serverPort;

	UserCmd userInput;

	ServerResponse response;
	ClientRequest request;

	void pasv();
	void actv();
	void ls();
	void get();
	void dir();
	void mkdir();
	void rmdir();
	void pwd();
	void exit();
	void del();
	void put();
	void cd();
	void mget();
	void mput();
	void mdelete();
	void lcd();
	void closeDataSocket();

public:
	FTPClient(std::string serverIP, int serverPort);
	~FTPClient();

	bool start();
	void talk();
};