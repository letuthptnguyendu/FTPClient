#pragma once

#include "import.h"
#include <algorithm>
#include "ClientRequest.h"
#include <string>
#include <vector>

class UserCmd: public ClientRequest
{
public:
	UserCmd();
	~UserCmd();

	virtual void assign(std::string full_cmd);
};