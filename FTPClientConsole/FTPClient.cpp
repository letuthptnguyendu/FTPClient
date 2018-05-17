#include "FTPClient.h"

FTPClient::FTPClient(std::string serverIP, int serverPort)
{
	this->serverIP = serverIP;
	this->serverPort = serverPort;

	//default mode is active
	mode = 0;
	subport = 0;

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
	std::cout << "welcome to }│@P│¢┴£╩E╝◄φj╪Ç║║╖║┴'s FTPClient\n";
	std::cout << "    ║║            ║║\n";
	std::cout << "    ║║            ║║\n";
	std::cout << "    ║║            ║║\n";
	std::cout << "   \\../          \\../\n";
	std::cout << "    \\/            \\/\n";
	//control socket connect to serverIP at serverPort
	this->controlSocket = new ClientSocket();
	controlSocket->connectTo(serverIP, serverPort);

	//220 open connect	
	response.assign(controlSocket->getReponse());
	if (response.getCode() == 220) {

		bool flag = FALSE;
		do {
			if (flag == TRUE) {
				std::cout << "re-enter username: ";
			}
			else {
				std::cout << "Username:";
			}
			/*--------------USERNAME---------------------*/
			std::getline(std::cin, username, '\n');
			/*--------------USERNAME---------------------*/

			std::string cmd = "USER " + username + '\n';
			request.assign(cmd);
			controlSocket->sendRequest(request);

			response.assign(controlSocket->getReponse());

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
				flag = TRUE;
			}
		} while (response.getCode() == 530);

	}
	else {
		throw "server can't be reached";
		return FALSE;
	}

	return TRUE;
}

void FTPClient::talk() {
	int cont = 1;
	std::string cmd;

	while (cont) {
		std::cout << "ANHDEPTRAI's FTP>";
		std::getline(std::cin, cmd, '\n');

		cmd += '\n';
		userInput.assign(cmd);

		if (userInput.getCmd() == "PASV") {
			pasv();
		}
		else if (userInput.getCmd() == "ACTV") {
			actv();
		}
		else if (userInput.getCmd() == "LS") {
			ls();
		}
		else if (userInput.getCmd() == "DIR") {
			dir();
		}
		else if (userInput.getCmd() == "GET") {
			get();
		}
		else if (userInput.getCmd() == "PUT") {
			put();
		}
		else if (userInput.getCmd() == "MKDIR") {
			mkdir();
		}
		else if (userInput.getCmd() == "RMDIR") {
			rmdir();
		}
		else if (userInput.getCmd() == "PWD") {
			pwd();
		}
		else if (userInput.getCmd() == "DELETE") {
			del();
		}
		else if (userInput.getCmd() == "CD") {
			cd();
		}
		else if (userInput.getCmd() == "LCD") {
			lcd();
		}
		else if (userInput.getCmd() == "MGET") {
			mget();
		}
		else if (userInput.getCmd() == "MPUT") {
			mput();
		}
		else if (userInput.getCmd() == "MDELETE") {
			mdelete();
		}
		else if (userInput.getCmd() == "QUIT" || userInput.getCmd() == "EXIT") {
			exit();
			cont = 0;
		}
	}
}

void FTPClient::pasv() {
	std::cout << "Enter-ing Passive Mode\n";
	mode = 1;	//pasv
	request.assign("PASV\n");
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	std::cout << response.getData();
	if (response.getCode() == 227) {
		int transferPort = response.getPort();

		if (dataSocket != NULL) {
			delete dataSocket;
		}
		this->dataSocket = new ClientSocket();
		dataSocket->connectTo(serverIP, transferPort);
	}
	else {
		throw "Can't open pasv mode";
	}
}

void FTPClient::actv() {
	mode = 0;
	std::cout << "Enter Active Mode\n";
	subport++;
	int port = controlSocket->getSocketPort() + subport;
	dataSocket = new ClientSocket(port, "127.0.0.1");

	int a = port / 256;
	int b = port % 256;
	std::string req = "PORT 127,0,0,1," + std::to_string(a) + "," + std::to_string(b) + "\n";
	request.assign(req);
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.getCode() == 200) {
		std::cout << response.getData();
	}
	else {
		throw "Can't open actv mode";
	}
}


void FTPClient::dir() {
	if (this->dataSocket == NULL) {
		actv();
	}
	if (mode == 0) {
		dataSocket->listenSock();
	}

	request.assign("LIST" + userInput.getConcatArgs() + "\n");
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	std::cout << response.getData();
	if (response.getCode() == 150) {
		if (mode == 0) {
			dataSocket->acceptSock(serverIP, serverPort - 1/*20*/);
		}
		std::string lsResponse = dataSocket->getReponse();
		while (lsResponse != "") {
			std::cout << lsResponse;
			lsResponse = dataSocket->getReponse();
		}
		closeDataSocket();
	}
	response.assign(controlSocket->getReponse());
	std::cout << response.getData();
}


void FTPClient::ls() {
	if (this->dataSocket == NULL) {
		actv();
	}
	if (mode == 0) {
		dataSocket->listenSock();
	}
	
	request.assign("NLST" + userInput.getConcatArgs() + "\n");
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	std::cout << response.getData();
	if (response.getCode() == 150) {
		if (mode == 0) {
			dataSocket->acceptSock(serverIP, serverPort - 1/*20*/);
		}
		std::string lsResponse = dataSocket->getReponse();
		while (lsResponse != "") {
			std::cout << lsResponse;
			lsResponse = dataSocket->getReponse();
		}
		closeDataSocket();
	}
	response.assign(controlSocket->getReponse());
	std::cout << response.getData();
}

void FTPClient::get() {
	if (this->dataSocket == NULL) {
		actv();
	}
	if (mode == 0) {
		dataSocket->listenSock();
	}
	request.assign("RETR" + userInput.getConcatArgs() + '\n');
	controlSocket->sendRequest(request);
	response.assign(controlSocket->getReponse());
	std::cout << response.getData();

	if (response.getCode() == 150) {
		if (mode == 0) {
			dataSocket->acceptSock(serverIP, serverPort - 1/*20*/);
		}
		std::string lsResponse = dataSocket->getReponse();

		std::string fileName = userInput.getConcatArgs().substr(1);
		std::ofstream outFile(fileName.c_str(), std::ios::binary | std::ios::out);

		while (lsResponse != "") {
			outFile << lsResponse;
			lsResponse = dataSocket->getReponse();
		}

		closeDataSocket();
		outFile.close();

		response.assign(controlSocket->getReponse());
		std::cout << response.getData();
	}
}

void FTPClient::put() {
	if (this->dataSocket == NULL) {
		actv();
	}
	if (mode == 0) {
		dataSocket->listenSock();
	}
	request.assign("STOR" + userInput.getConcatArgs() + '\n');
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	std::cout << response.getData();
	if (response.getCode() == 150) {
		if (mode == 0) {
			dataSocket->acceptSock(serverIP, serverPort - 1/*20*/);
		}
		std::string fileName = userInput.getConcatArgs().substr(1);
		std::ifstream inpFile(fileName.c_str(), std::ios::binary | std::ios::in);

		if (!inpFile) {
			throw "Can't Open file to Transfer";
		}

		std::string dataSend;
		while (!inpFile.eof()) {
			std::getline(inpFile, dataSend, '\n');
			dataSend += '\n';
			dataSocket->sendData(dataSend);
		}

		closeDataSocket();
		inpFile.close();

		response.assign(controlSocket->getReponse());
		std::cout << response.getData();
	}
	else {
		throw "Can't Send";
	}
}

void FTPClient::mkdir() {
	request.assign("XMKD" + userInput.getConcatArgs() + '\n');
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.getCode() == 257) {
		std::cout << response.getData();
	}
	else {
		//????
	}
}

void FTPClient::rmdir() {
	request.assign("XRMD" + userInput.getConcatArgs() + '\n');
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.getCode() == 250) {
		std::cout << response.getData();
	}
	else {
		if (response.getCode() == 550) {
			//not empty
		}
		else {
			//fail
		}
	}
}

void FTPClient::pwd() {
	request.assign("XPWD\n");
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.getCode() == 257) {
		std::cout << response.getData() << std::endl;
	}
	else {
		//fail
	}
}

void FTPClient::exit() {
	request.assign("QUIT\n");
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	std::cout << response.getData();
	if (response.getCode() == 221) {
		//bye
	}
	else {
		//fail
	}
}

void FTPClient::del() {
	request.assign("DELE" + userInput.getConcatArgs() + '\n');
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.getCode() == 250) {
		std::cout << response.getData();
	}
	else {
		//fail
	}
}

void FTPClient::cd() {
	request.assign("CWD" + userInput.getConcatArgs() + '\n');
	controlSocket->sendRequest(request);

	response.assign(controlSocket->getReponse());
	if (response.getCode() == 250) {
		std::cout << response.getData();
	}
	else if (response.getCode() == 550) {
		std::cout << response.getData();
	}
	else {
		//another reason
	}
}

void FTPClient::lcd() {
	const int bufferSize = 2048;
	char oldDir[bufferSize];
	
	if (userInput.getConcatArgs() == "") {
		GetCurrentDirectory(bufferSize, oldDir);
		std::cout << "Current Directroy is: " << oldDir << '\n';
	}
	else {
		std::string dir = userInput.getConcatArgs().substr(1);
		SetCurrentDirectory(dir.c_str());
	}

	GetCurrentDirectory(bufferSize, oldDir);
	std::cout << "Current Directroy is: " << oldDir << '\n';
}

void FTPClient::closeDataSocket() {
	dataSocket->~ClientSocket();
	dataSocket = NULL;
}

void FTPClient::mget() {
	std::vector<std::string> args = userInput.getArgs();
	for (int i = 0; i < args.size(); i++) {
		userInput.assign("LS " + args[i] + "\n");
		ls();
		userInput.assign("GET " + args[i] + "\n");
		get();
	}
}

void FTPClient::mput() {  
	std::vector<std::string> args = userInput.getArgs();
	for (int i = 0; i < args.size(); i++) {
		userInput.assign("LS " + args[i] + "\n");
		ls();
		userInput.assign("PUT " + args[i] + "\n");
		put();
	}
}

void FTPClient::mdelete() {
	std::vector<std::string> args = userInput.getArgs();
	for (int i = 0; i < args.size(); i++) {
		userInput.assign("LS " + args[i] + "\n");
		ls();
		userInput.assign("DELETE " + args[i] + "\n");
		del();
	}
}