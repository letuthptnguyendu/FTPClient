#include "ClientRequest.h"

ClientRequest::ClientRequest()
{
}

ClientRequest::~ClientRequest()
{
}

void ClientRequest::clean()
{
	args.clear();
	cmd = "";
}

void ClientRequest::assign(std::string full_cmd)
{
	this->clean();
	this->full_cmd = full_cmd;

	int end_cmd_pos;
	if (this->full_cmd.find_first_of(' ', 0) != std::string::npos) {
		end_cmd_pos = full_cmd.find_first_of(' ', 0);
		this->cmd = this->full_cmd.substr(0, end_cmd_pos - 0);//get cmd

		std::string temp;
		full_cmd = full_cmd.substr(end_cmd_pos + 1);

		while (full_cmd.find_first_of('"') != std::string::npos
				|| full_cmd.find_first_of(' ') != std::string::npos) {
			if (full_cmd[0] == '"') {
				args.push_back(full_cmd.substr(1, full_cmd.find_first_of('"', 1) - 1));
				full_cmd = full_cmd.substr(full_cmd.find_first_of('"', 1) + 2);
			}
			else {
				args.push_back(full_cmd.substr(0, full_cmd.find_first_of(' ', 0)));
				full_cmd = full_cmd.substr(full_cmd.find_first_of(' ', 1) + 1);
			}
		}

		if (full_cmd.length() != 0) { args.push_back(full_cmd.substr(0, full_cmd.find_first_of('\n'))); }//last
	}
	else {
		if (full_cmd.length() != 0) {
			this->cmd = this->full_cmd.substr(0, full_cmd.find_first_of('\n'));//get cmd
		}
	}
}

std::string ClientRequest::getFullCmd() {
	return this->full_cmd;
}

std::string ClientRequest::getCmd() {
	return this->cmd;
}

std::vector<std::string> ClientRequest::getArgs() {
	return this->args;
}

void ClientRequest::setArgs(std::vector<std::string> files) {
	args = files;
}