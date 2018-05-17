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
	}
	else {
		end_cmd_pos = full_cmd.find_first_of('\n', 0);
	}
	this->cmd = this->full_cmd.substr(0, end_cmd_pos - 0);//get cmd

	int next_arg_pos = end_cmd_pos;
	int prev_pos = end_cmd_pos;
	std::string arg;
	while (this->full_cmd.find_first_of(' ', next_arg_pos + 1) != std::string::npos) {
		next_arg_pos = this->full_cmd.find_first_of(' ', next_arg_pos + 1);
		arg = this->full_cmd.substr(prev_pos + 1, next_arg_pos - prev_pos - 1);
		this->args.push_back(arg);
		prev_pos = next_arg_pos;
	}

	//last arg
	next_arg_pos = this->full_cmd.find_first_of('\n', prev_pos + 1);
	arg = this->full_cmd.substr(prev_pos + 1, next_arg_pos - prev_pos - 1);
	this->args.push_back(arg);
}

std::string ClientRequest::getFullCmd() {
	return this->full_cmd;
}

std::string ClientRequest::getCmd() {
	return this->cmd;
}

std::string ClientRequest::getConcatArgs() {
	std::string result = "";
	for (int i = 0; i < args.size(); i++) {
		result += (" " + args[i]);
	}
	return result;
}

std::vector<std::string> ClientRequest::getArgs() {
	return this->args;
}