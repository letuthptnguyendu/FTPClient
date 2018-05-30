#include "utilities.h"


std::string getPassword() {
	std::string pass = "";
	char ch;
	do
	{
		ch = _getch();
		pass += (char)ch;
		if (ch != '\r') { std::cout << '*'; }
	} while (ch != '\r');

	return pass;
}

bool validateIP(std::string IP) {
	bool result = true;

	int pos = -1;
	int countDot = 0;

	std::string IPpart_str;
	int IPpart_num;

	while (IP.find_first_of('.', pos + 1) != std::string::npos) {
		IPpart_str = IP.substr(0, IP.find_first_of('.', pos + 1) - 1);
		IPpart_num = stoi(IPpart_str);

		if (IPpart_num < 0 ||
			IPpart_num > 255)
		{
			result = false;
		}

		pos = IP.find('.', pos + 1);
		countDot++;
	}
	if (countDot != 3) { result = false; }

	return result;
}

void takeFileName(std::string &directory) {
	if (directory.find_last_of('/') != std::string::npos) {
		directory = directory.substr(directory.find_last_of('/') + 1);
	}
}

void removeCarriageReturn(std::string &directory) {
	if (directory.find('\r') != std::string::npos) {
		directory = directory.substr(0, directory.find('\r'));
	}
}

void unquote(std::string &directory) {
	if (directory[0] == '\"') {
		directory = directory.substr(1, directory.length() - 1);
	}
	if (directory[directory.length() - 1] == '\"') {
		directory = directory.substr(0, directory.length() - 1);
	}
}

void quote(std::string &directory) {
	if (directory[0] != '\"') {
		directory = '\"' + directory;
	}
	if (directory[directory.length() - 1] != '\"') {
		directory = directory + '\"';
	}
}

void handleSpecialCharacterToPrint(std::string &directory) {
	int pos = -1;
	while (directory.find_first_of('/', pos + 1) != std::string::npos) {
		pos = directory.find_first_of('/', pos + 1) + 2;
		directory.replace(pos - 2, 1, "\/");
	}
}