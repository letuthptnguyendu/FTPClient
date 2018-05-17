#include "ServerResponse.h"

ServerResponse::ServerResponse()
{
}


ServerResponse::~ServerResponse()
{
}

void ServerResponse::assign(std::string input) {
	this->data = input;
	this->code = std::stoi(data.substr(0, 3));
}

int ServerResponse::getCode() {
	return this->code;
}

int ServerResponse::getPort() {
	if (this->code == 227) {
		int begin = data.find_first_of('(', 0);
		int runner = 0;
		for (int i = 0; i < 4; i++) {
			begin = data.find_first_of(',', begin + 1);
		}

		int begin2 = data.find_first_of(',', begin + 1);
		int end = data.find_first_of(')', begin2 + 1);

		int num1 = stoi(data.substr(begin + 1, begin2 - begin - 1));
		int num2 = stoi(data.substr(begin2 + 1, end - begin2 - 1));

		return num1 * 256 + num2;
	}
	else {
		return -1;
	}
}

std::string ServerResponse::getData() {
	return this->data;
}