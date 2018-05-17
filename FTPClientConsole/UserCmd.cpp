#include "UserCmd.h"



UserCmd::UserCmd()
{
}


UserCmd::~UserCmd()
{
}


void UserCmd::assign(std::string full_cmd) {
	ClientRequest::assign(full_cmd);

	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
}