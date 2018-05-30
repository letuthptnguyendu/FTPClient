#pragma once

/*Client Request
	FTP form   */

#include "import.h"

class ClientRequest
{
protected:
	std::string full_cmd;

	std::string cmd;
	std::vector<std::string> args;

public:
	ClientRequest();
	~ClientRequest();

	void clean();
	virtual void assign(std::string full_cmd);
	
	std::string getFullCmd();
	std::string getCmd();
	std::vector<std::string> getArgs();
	void setArgs(std::vector<std::string> files);
};