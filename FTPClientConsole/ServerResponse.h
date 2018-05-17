#pragma once

#include "import.h"
#include "ClientSocket.h"
#include <string>

class ServerResponse
{
private: 
	std::string data;
	int code;

public:
	ServerResponse();
	~ServerResponse();

	void assign(std::string input);
	int getCode();
	int getPort();
	std::string getData();
};

