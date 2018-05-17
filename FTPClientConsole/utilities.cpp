#include "utilities.h"

std::string getPassword() {
	std::string pass = "";
	char ch;
	do
	{
		ch = _getch();
		pass += (char)ch;
		if (ch != '\r') { std::cout << 'x'; }
	} while (ch != '\r');

	return pass;
}
