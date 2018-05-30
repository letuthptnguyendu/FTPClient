#pragma once

#include "UserCmd.h"
#include "utilities.h"
#include "ClientSocket.h"
#include "ClientRequest.h"
#include "ServerResponse.h"

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
	clock_t client_clock;

	std::string serverIP;
	int	serverPort;

	UserCmd userInput;

	ServerResponse response;
	ClientRequest request;

	void pasv();
	void actv();
	bool ls_dir();
	void put();
	void get();
	void mkdir();
	void lcd();
	void rmdir();
	void pwd();
	void exit();
	void del();
	void cd();
	void mget();
	void mput();
	void mdelete();
	void getFolder();
	void delFolder();

	void closeDataSocket();
	void checkAndInitDataSocket();
public:
	FTPClient(std::string serverIP, int serverPort);
	~FTPClient();

	bool start();
	void talk();
};