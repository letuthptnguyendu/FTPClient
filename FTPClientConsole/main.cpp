#include "FTPClient.h"

int main() {
	FTPClient client("127.0.0.1", 21);

	if (client.start() ){
		client.talk();
	}

	return 0;
}