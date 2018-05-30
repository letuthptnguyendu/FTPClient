#pragma once

#include "import.h"
#include "ClientSocket.h"

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

	bool check226();
	void printTo226();
	void print226();

	void printData();
};

