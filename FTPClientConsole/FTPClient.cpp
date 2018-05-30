#include "FTPClient.h"

FTPClient::FTPClient(std::string serverIP, int serverPort)
{
	this->serverIP = serverIP;
	this->serverPort = serverPort;

	//default mode is active
	mode = 0;
	subport = 5;

	controlSocket = NULL;
	dataSocket = NULL;
}

FTPClient::~FTPClient()
{
	if (controlSocket) {
		controlSocket->~ClientSocket();
		delete controlSocket;
	}
	if (dataSocket) {
		dataSocket->~ClientSocket();
		delete dataSocket;
	}
}

bool FTPClient::start() {
	std::cout << "::::Welcome to my FTPClient::::\n";
	std::cout << "     ║║            ║║\n";
	std::cout << "     ║║            ║║\n";
	std::cout << "    \\../          \\../\n";
	std::cout << "     \\/            \\/\n";

	try {
		this->controlSocket = new ClientSocket();
		controlSocket->connectTo(serverIP, serverPort);
	}
	catch (std::string e) {
		std::cout << "EEEEEEEEERRRRRRRRROOOOORRRRRRRR: " << e << std::endl;
	}

	response.assign(controlSocket->getReponse());
	if (response.getCode() == 220) {
		bool flag = FALSE;//to control user input
		do {
			if (flag == TRUE) {
				std::cout << "re-enter username: ";
			}
			else {
				std::cout << "Username:";
			}
			/*--------------USERNAME---------------------*/
			username = "";
			while (username == "") {
				std::getline(std::cin, username, '\n');
			}
			/*--------------USERNAME---------------------*/

			std::string cmd = "USER " + username + '\n';
			request.assign(cmd);

			controlSocket->sendRequest(request);
			response.assign(controlSocket->getReponse());
			response.printData();

			if (response.getCode() == 331) {
				if (flag == TRUE) {
					std::cout << "re-enter password: ";
				}
				else {
					std::cout << "Password:";
				}
				/*--------------PASSWORD---------------------*/
				password = getPassword();
				std::cout << std::endl;
				/*--------------PASSWORD---------------------*/

				std::string cmd = "PASS " + password + '\n';
				request.assign(cmd);

				controlSocket->sendRequest(request);
				response.assign(controlSocket->getReponse());
				response.printData();

				flag = TRUE;//1st is wrong
			}
		} while (response.getCode() == 530);

		std::cout << std::endl;
		client_clock = clock();
		//START THE CLOCK
	}
	else {
		std::string e = "server can't be reached\n";
		throw e;
		return FALSE;
	}
	return TRUE;
}

void FTPClient::talk() {
	int cont = 1;
	std::string cmd = "";//user input here

	while (cont) {
		cmd = "";
		std::cout << "Wi's FTP>";

		while (cmd == "") {
			std::getline(std::cin, cmd, '\n');
		}


		cmd += '\n';
		userInput.assign(cmd);

		try {
			if ((clock() - client_clock) / CLOCKS_PER_SEC > 119) {
				std::cout << "Time out\n";
				cont = 0;
				Sleep(2000);
				return;
			}//CHECK CLOCK

			if (userInput.getCmd() == "PASV") {
				pasv();
			}
			else
			{
				if (userInput.getCmd() == "MKDIR") { mkdir(); }
				else if (userInput.getCmd() == "RMDIR") { rmdir(); }
				else if (userInput.getCmd() == "PWD") { pwd(); }
				else if (userInput.getCmd() == "DELETE") { del(); }
				else if (userInput.getCmd() == "CD") { cd(); }
				else if (userInput.getCmd() == "LCD") { lcd(); }
				else if (userInput.getCmd() == "MDELETE") { mdelete(); }
				else if (userInput.getCmd() == "QUIT" || userInput.getCmd() == "EXIT") {
					exit();
					cont = 0;
				}

				else if (userInput.getCmd() == "MGET" ||
					userInput.getCmd() == "MPUT" ||
					userInput.getCmd() == "LS" ||
					userInput.getCmd() == "DIR" ||
					userInput.getCmd() == "GET" ||
					userInput.getCmd() == "PUT")
				{
					//datasock-related command
					try {
						if (userInput.getCmd() == "MGET") { mget(); }
						else if (userInput.getCmd() == "MPUT") { mput(); }
						else if (userInput.getCmd() == "LS" ||
							userInput.getCmd() == "DIR") {
							ls_dir();
						}
						else if (userInput.getCmd() == "GET") { get(); }
						else if (userInput.getCmd() == "PUT") { put(); }
						mode = 0;
					}
					catch (std::string e) {
						std::cout << e;
					}
					closeDataSocket();
				}
				else {
					std::cout << "Invalid command!\n";
				}

			}

			client_clock = clock();//RESTART CLOCK
			std::cout << '\n';
		}
		catch (std::string e) {
			std::cout << "EEEEEEEEERRRRRRRRROOOOORRRRRRRR: " << e << std::endl;
		}
	}
}

void FTPClient::pasv() {
	std::cout << "::::Enter-ing Passive Mode\n";

	mode = 1;	//pasv

	request.assign("PASV\n");
	controlSocket->sendRequest(request);
	response.assign(controlSocket->getReponse());
	response.printData();

	if (response.getCode() == 227) {
		int transferPort = response.getPort();
		this->dataSocket = new ClientSocket();
		dataSocket->connectTo(serverIP, transferPort);
	}
	else {
		std::string e = "Can't initialize pasv mode\n";
		throw e;
	}
}

void FTPClient::actv() {
	std::cout << "::::Enter Active Mode\n";
	//initialize
	mode = 0;
	subport += 10;//here is where initialize subport 

	//generate a new port in client
	int port = controlSocket->getSocketPort() + subport;
	//get IP client
	std::string clientIP = controlSocket->getSocketIP();
	//create socket to PORT cmd
	dataSocket = new ClientSocket(clientIP, port);

	//IP STRING PROCESSING
	for (int i = 0, pos = -1; i < 3; i++) {
		clientIP = clientIP.replace(clientIP.find_first_of('.', pos + 1), 1, ",");
		pos = clientIP.find_first_of('.', pos);
	}

	int a = port / 256;
	int b = port % 256;

	std::string req = "PORT " + clientIP + "," + std::to_string(a) + "," + std::to_string(b) + "\n";
	request.assign(req);

	controlSocket->sendRequest(request);
	response.assign(controlSocket->getReponse());
	if (response.getCode() == 200) {
		response.printData();
	}
	else {
		std::string e = "Can't initialize actv mode\n";
		throw e;
	}
}

bool FTPClient::ls_dir() {
	checkAndInitDataSocket();
	dataSocket->listenSock();

	std::string file;
	if (userInput.getArgs().size() != 0) { file = userInput.getArgs()[0]; }

	if (userInput.getCmd() == "LS") {
		request.assign("NLST " + file + "\n");
	}
	else if (userInput.getCmd() == "DIR") {
		request.assign("LIST " + file + "\n");
	}

	controlSocket->sendRequest(request);
	response.assign(controlSocket->getReponse());
	if (response.check226()) { response.printTo226(); }
	else { response.printData(); }

	if (response.getCode() == 150 || response.getCode() == 125) {
		if (mode == 0) {
			dataSocket->acceptSock(serverIP, 20);
		}

		std::string lsResponse = dataSocket->getReponse();
		while (lsResponse != "") {
			std::cout << lsResponse;
			lsResponse = dataSocket->getReponse();
		}
		closeDataSocket();
	}
	else {
		if (response.getCode() == 550) {
			std::string e = "File not Found\n";
			throw e;
		}
		else {
			std::string e = "Ls_dir Error\n";
			throw e;
		}
		return false;
	}

	if (response.check226()) { response.print226(); }
	else {
		response.assign(controlSocket->getReponse());
		response.printData();
	}

	return true;
}

void FTPClient::get() {
	clock_t t;
	checkAndInitDataSocket();
	dataSocket->listenSock();
	std::string file;
	if (userInput.getArgs().size() != 0) { file = userInput.getArgs()[0]; }
	else {
		std::cout << "Remote file name ? ";
		std::getline(std::cin, file, '\n');
	}

	request.assign("RETR " + file + '\n');
	controlSocket->sendRequest(request);
	response.assign(controlSocket->getReponse());
	if (response.check226()) { response.printTo226(); }
	else { response.printData(); }

	if (response.getCode() == 150 || response.getCode() == 125) {

		if (mode == 0) {
			dataSocket->acceptSock(serverIP, 20);
		}

		//take fileName if directory has folder
		takeFileName(file);
		removeCarriageReturn(file);
		unquote(file);

		std::ofstream outFile(file.c_str(), std::ios::binary | std::ios::out);
		long fileSize;
		if (!outFile)
		{
			std::string e = "File error\n";
			throw e;
		}
		else {
			t = clock();

			std::string lsResponse = dataSocket->getReponse();
			fileSize = lsResponse.length();
			while (lsResponse != "") {
				outFile << lsResponse;
				lsResponse = dataSocket->getReponse();
				fileSize += lsResponse.length();
			}
		}

		t = clock() - t;
		std::cout << "byte in total: " << fileSize << "\n";
		std::cout << "time recieve: " << ((float)t) / CLOCKS_PER_SEC << "\n";
		std::cout << "average recieving rate:" << fileSize / ((float)t) / CLOCKS_PER_SEC << "bytes / secs" << "\n";

		closeDataSocket();
		outFile.close();

		if (response.check226()) { response.print226(); }
		else {
			response.assign(controlSocket->getReponse());
			response.printData();
		}
	}
	else {
		if (response.getCode() == 550) {
			std::string e = "File not Found\n";
			throw e;
		}
		else {
			std::string e = "Get Error\n";
			throw e;
		}
	}
}

void FTPClient::put() {
	clock_t t;
	checkAndInitDataSocket();
	dataSocket->listenSock();

	std::string file;
	if (userInput.getArgs().size() != 0) { file = userInput.getArgs()[0]; }
	else {
		std::cout << "File name ? ";
		std::getline(std::cin, file, '\n');
	}

	//take fileName if directory has folder
	takeFileName(file);
	removeCarriageReturn(file);
	unquote(file);
	std::ifstream inpFile(file.c_str(), std::ios::binary | std::ios::in);

	if (!inpFile) {
		std::string e = "File Error\n";
		throw e;
	}

	request.assign("STOR " + file + '\n');
	controlSocket->sendRequest(request);
	response.assign(controlSocket->getReponse());
	if (response.check226()) { response.printTo226(); }
	else { response.printData(); }

	if (response.getCode() == 150 || response.getCode() == 125) {
		if (mode == 0) {
			dataSocket->acceptSock(serverIP, 20);
		}

		const int readSize = 2048;
		char readBuffer[readSize];
		std::string dataSend;

		inpFile.seekg(0, inpFile.end);
		int length = inpFile.tellg();
		inpFile.seekg(0, inpFile.beg);
		int length_temp = length;

		t = clock();
		if (length_temp != 0)
			while (!inpFile.eof()) {
				if (length_temp > readSize) {
					inpFile.read(readBuffer, readSize);
					dataSend.assign(readBuffer, readSize);
					length_temp -= readSize;
				}
				else {
					inpFile.read(readBuffer, length_temp);
					dataSend.assign(readBuffer, length_temp);
				}

				dataSocket->sendData(dataSend);
			}

		t = clock() - t;
		std::cout << "byte in total: " << length << "\n";
		std::cout << "time recieve: " << ((float)t) / CLOCKS_PER_SEC << "\n";
		std::cout << "average sending rate:" << length / ((float)t) / CLOCKS_PER_SEC << "bytes / secs" << "\n";

		closeDataSocket();
		inpFile.close();

		if (response.check226()) { response.print226(); }
		else {
			response.assign(controlSocket->getReponse());
			response.printData();
		}
	}
	else {
		std::string e = "Can't open tranfer channel\n";
		throw e;
	}
}

void FTPClient::mkdir() {
	std::string directory;
	if (userInput.getArgs().size() != 0) {
		directory = userInput.getArgs()[0];
	}
	else {
		std::cout << "Directory ? ";
		std::getline(std::cin, directory, '\n');
	}

	request.assign("XMKD " + directory + '\n');
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.getCode() == 257) {
		response.printData();
	}
	else {
		std::string e = "mkdir Error\n";
		throw e;
	}
}

void FTPClient::rmdir() {
	std::string directory;
	if (userInput.getArgs().size() != 0) {
		directory = userInput.getArgs()[0];
	}
	else {
		std::cout << "Directory ? ";
		std::getline(std::cin, directory, '\n');
	}

	request.assign("XRMD " + directory + '\n');
	controlSocket->sendRequest(request);
	response.assign(controlSocket->getReponse());

	if (response.getCode() == 250) {
		response.printData();
	}
	else {
		if (response.getCode() == 550) {
			std::string e = "Folder not empty\n";
			throw e;
		}
		else {
			std::string e = "rmdir Error\n";
			throw e;
		}
	}
}

void FTPClient::pwd() {
	request.assign("XPWD\n");
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.getCode() == 257) {
		response.printData();
	}
	else {
		std::string e = "pwd Error\n";
		throw e;
	}
}

void FTPClient::exit() {
	request.assign("QUIT\n");
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	response.printData();
	if (response.getCode() != 221) {
		std::string e = "bye Error\n";
		throw e;
	}
}

void FTPClient::del() {
	std::string file;
	if (userInput.getArgs().size() != 0) {
		file = userInput.getArgs()[0];
	}
	else {
		std::cout << "Remote file name ? ";
		std::getline(std::cin, file, '\n');
	}

	request.assign("DELE " + file + '\n');
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.getCode() == 250) {
		response.printData();
	}
	else {
		std::string e = "del Error\n";
		throw e;
	}
}

void FTPClient::cd() {
	std::string directory;
	if (userInput.getArgs().size() != 0) {
		directory = userInput.getArgs()[0];
	}
	else {
		std::cout << "Directory ? ";
		std::getline(std::cin, directory, '\n');
	}

	request.assign("CWD " + directory + '\n');
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.getCode() == 250) {
		response.printData();
	}
	else if (response.getCode() == 550) {
		response.printData();
	}
	else {
		std::string e = "cd Error\n";
		throw e;
	}
}

void FTPClient::lcd() {
	const int bufferSize = 2048;
	char oldDir[bufferSize];

	if (userInput.getArgs().size() == 0) {
		GetCurrentDirectory(bufferSize, oldDir);
		std::cout << "Current Directroy is: " << oldDir << '\n';
	}
	else {
		std::string dir = userInput.getArgs()[0];
		SetCurrentDirectory(dir.c_str());
		GetCurrentDirectory(bufferSize, oldDir);
		std::cout << "Current Directroy is: " << oldDir << '\n';
	}
}

void FTPClient::closeDataSocket() {
	if (dataSocket) {
		dataSocket->~ClientSocket();
		delete dataSocket;
		dataSocket = NULL;
	}
}

void FTPClient::mget() {
	std::vector<std::string> args_temp = userInput.getArgs();
	int size = args_temp.size();

	char yes;
	for (int i = 0; i < size; i++) {
		if (args_temp[i].find('.') != std::string::npos) {
			//if file
			std::string fileName = args_temp[i];

			handleSpecialCharacterToPrint(fileName);
			removeCarriageReturn(fileName);

			std::cout << "\nget file " + fileName + "? (\'y\' or \'n\'):";
			std::cin >> yes;

			if (yes == 'y') {
				quote(args_temp[i]);
				userInput.assign("GET " + args_temp[i] + "\n");
				try {
					get();
				}
				catch (std::string e) {
					std::cout << e;
					closeDataSocket();
				}
			}
		}
		else {
			std::string folder = args_temp[i];

			handleSpecialCharacterToPrint(folder);
			removeCarriageReturn(folder);

			std::cout << "\nget folder " + folder + "? (\'y\' or \'n\'):";

			std::cin >> yes;
			if (yes == 'y') {
				takeFileName(folder);
				quote(folder);
				quote(args_temp[i]);

				std::string cmd1 = "mkdir " + folder + "\n";
				system(cmd1.c_str());

				unquote(folder);
				SetCurrentDirectory(folder.c_str());
				userInput.assign("GET " + args_temp[i] + "\n");
				try {
					getFolder();
				}
				catch (std::string e) {
					std::cout << e;
					closeDataSocket();
				}
				SetCurrentDirectory("..");
			}
		}

		closeDataSocket();
	}
}

void FTPClient::mput() {
	std::vector<std::string> args_temp = userInput.getArgs();
	int size = args_temp.size();

	char yes;
	for (int i = 0; i < size; i++) {
		std::string fileName = args_temp[i];

		handleSpecialCharacterToPrint(fileName);
		removeCarriageReturn(fileName);

		std::cout << "\nput file " + fileName + "? (\'y\' or \'n\'):";
		std::cin >> yes;

		if (yes == 'y') {
			quote(args_temp[i]);
			userInput.assign("PUT " + args_temp[i] + "\n");
			try {
				put();
			}
			catch (std::string e) {
				std::cout << e;
				closeDataSocket();
			}
		}
	}
}

void FTPClient::mdelete() {
	std::vector<std::string> args_temp = userInput.getArgs();
	int size = args_temp.size();

	char yes;
	for (int i = 0; i < size; i++) {
		if (args_temp[i].find('.') != std::string::npos) {
			//if file
			std::string fileName = args_temp[i];

			handleSpecialCharacterToPrint(fileName);
			removeCarriageReturn(fileName);

			std::cout << "\ndelete file " + fileName + "? (\'y\' or \'n\'):";
			std::cin >> yes;

			if (yes == 'y') {
				quote(args_temp[i]);
				userInput.assign("DELETE " + args_temp[i] + "\n");
				del();
			}
		}
		else {
			std::string folder = args_temp[i];
			handleSpecialCharacterToPrint(folder);
			removeCarriageReturn(folder);

			std::cout << "\ndelete folder " + folder + "? (\'y\' or \'n\'):";
			std::cin >> yes;
			if (yes == 'y') {
				quote(folder);
				quote(args_temp[i]);

				userInput.assign("DELETE " + args_temp[i] + "\n");
				delFolder();

				userInput.assign("RMDIR " + folder + "\n");
				rmdir();
			}
		}
	}
}

void FTPClient::getFolder() {
	checkAndInitDataSocket();
	dataSocket->listenSock();

	request.assign("NLST " + userInput.getArgs()[0] + "\n");
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.check226()) { response.printTo226(); }
	else { response.printData(); }

	//take files into an array
	std::vector<std::string> files;
	if (response.getCode() == 150 || response.getCode() == 125) {
		if (mode == 0) {
			dataSocket->acceptSock(serverIP, 20);
		}

		std::string lsResponse = dataSocket->getReponse();
		while (lsResponse != "") {
			std::cout << lsResponse;

			while (lsResponse.find_first_of('\n') != std::string::npos) {
				files.push_back(lsResponse.substr(0, lsResponse.find_first_of('\n')));
				lsResponse = lsResponse.substr(lsResponse.find_first_of('\n') + 1);
			}

			lsResponse += dataSocket->getReponse();
		}
		closeDataSocket();

		if (response.check226()) { response.print226(); }
		else {
			response.assign(controlSocket->getReponse());
			response.printData();
		}

		userInput.setArgs(files);
		mget();
	}
	else {
		std::string e = "Can't open tranfer channel\n";
		throw e;
	}
}


void FTPClient::delFolder() {
	checkAndInitDataSocket();
	dataSocket->listenSock();

	request.assign("NLST " + userInput.getArgs()[0] + "\n");
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.check226()) { response.printTo226(); }
	else { response.printData(); }

	//take files in folder into an array
	std::vector<std::string> files;
	if (response.getCode() == 150 || response.getCode() == 125) {
		if (mode == 0) {
			dataSocket->acceptSock(serverIP, 20);
		}

		std::string lsResponse = dataSocket->getReponse();
		while (lsResponse != "") {
			std::cout << lsResponse;

			while (lsResponse.find_first_of('\n') != std::string::npos) {
				files.push_back(lsResponse.substr(0, lsResponse.find_first_of('\n')));
				lsResponse = lsResponse.substr(lsResponse.find_first_of('\n') + 1);
			}

			lsResponse += dataSocket->getReponse();
		}
		closeDataSocket();

		if (response.check226()) { response.print226(); }
		else {
			response.assign(controlSocket->getReponse());
			response.printData();
		}

		userInput.setArgs(files);
		mdelete();
	}
	else {
		std::string e = "Can't open tranfer channel\n";
		throw e;
	}
}

void FTPClient::checkAndInitDataSocket() {
	if (this->dataSocket == NULL) {
		if (mode == 1) { pasv(); }
		else { actv(); }
	}
}