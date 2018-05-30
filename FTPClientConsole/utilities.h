#include "import.h"


std::string getPassword();
bool validateIP(std::string IP);

void takeFileName(std::string &directory);
void removeCarriageReturn(std::string &directory);
void unquote(std::string &directory);
void quote(std::string &directory);

void handleSpecialCharacterToPrint(std::string &directory);