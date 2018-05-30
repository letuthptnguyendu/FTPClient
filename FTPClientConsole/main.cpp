#include "FTPClient.h"

int main(int argc, char* argv[]) {
	FTPClient* client = NULL;

	if (argc == 3) {
		std::string ipConnectTo = argv[1];
		
		if (validateIP) {
			
			client = new FTPClient(ipConnectTo, 21);
			try {
				if (client->start()) {
					client->talk();
				}
			}
			catch (std::string e) {
				std::cout << "EEEEEEEEERRRRRRRRROOOOORRRRRRRR: " << e << std::endl;
			}
			delete client;
		}
		else {
			std::cout << "IP sai roi\nKet thuc chuong trinh tai day!\n";
			return 0;
		}
	}

	return 1;
}