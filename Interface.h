#pragma once
#include <iostream>
#include <string>
#include <thread>

#include "DB.h"

class Interface {
public:
	std::queue<Message> in = {};
	std::queue<Message> out = {};
	Controller controller = Controller(&out, &in);
	std::thread backThread;

	Interface()
	{
		backThread = std::thread( controller.start );
	}

	void processMessage(Message msg)
	{
		if (msg.message.compare(""))
		{
			cout << msg.message;
			return;
		}

		std::string str;
		switch (msg.operation)
		{
		case ENCRYPT:
			str += "File \'" + msg.dataFilePath + "\' encrypted in " + std::to_string(msg.time) + "s.";
			break;
		case DECRYPT:
			str += "File \'" + msg.dataFilePath + "\' decrypted in " + std::to_string(msg.time) + "s.";
			break;
		case GEN_KEY:
			str += "Generated key in file \'" + msg.keyFilePath + "\'.";
			break;
		}
	}

	int optionChoice()
	{
		int option;
		string fileName;
		cout << "Choose one of the following: " << endl;
		cout << "1. Display Algorithms" << endl;
		cout << "2. Display keys" << endl;
		cout << "3. Display files" << endl;
		cout << "4. Insert file" << endl;
		cout << "5. Delete file" << endl;
		cout << "6. Encrypt existing file" << endl;
		cout << "7. Decrypt existing file" << endl;
		cout << "8. Exit" << endl;
		cin >> option;

		switch (option)
		{
		case 1:
			Database::selectData(db, option);
			break;
		case 2:
			Database::selectData(db, option);
			break;
		case 3:
			Database::selectData(db, option);
			break;
		case 4:
			insertFileInterface();
			break;
		case 5:
			deleteFileInterface();
			break;
		case 6:
			cout << "Select file:" << endl;
			Database::selectData(db, 3);
			cin >> fileName;
			Message msg(ENCRYPT, LIB_OpenGG, ALG_RSA_4096, "rsa4096.txt", fileName, 4096);
			out.push(msg);
			//cout << OpenGG::asymmetricEncrDecr("rsa4096.txt", fileName, true) << endl;
			break;
		case 7:
			cout << "Select file to decrypt: " << endl;
			Database::selectData(db, 3);
			cin >> fileName;
			cout << OpenGG::asymmetricEncrDecr("rsa4096.txt", fileName, false) << endl;
			break;
		case 8:
			backThread.join();
			return 0;
		default:
			cout << "Option doesn't exist" << endl;
			break;
		}
	}

	void insertFileInterface()
	{
		int key_id;
		string path;
		double encr_time = 0, decr_time = 0;
		bool status;

		cout << "File path (entire path): ";
		cin >> path;
		cout << "Key id: " << endl;
		cout << "Available key ids: " << endl;
		Database::selectData(db, 2);
		cin >> key_id;
		cout << "Encrypted? (true/false)" << endl;
		cin >> status;
		Database::insertFile(db, key_id, path, encr_time, decr_time, status);
	}

	void deleteFileInterface()
	{
		int file_id;

		cout << "Current files: " << endl;
		Database::selectData(db, 1);
		cout << "File id to delete: " << endl;
		cin >> file_id;

		Database::deleteFile(db, file_id);
	}
};